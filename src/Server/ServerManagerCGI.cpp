/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerCGI.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 16:20:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/17 12:23:44 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include "ServerManager.h"
#include "server/ServerExceptions.h"

void ServerManager::CheckCGIState(int client_socket) {
    Connection		&connection = connections_[client_socket];

    if (connection.waiting_for_cgi_) {
        DetachCGI(connection);
        connections_[client_socket] = Connection(is_running_, client_socket,
                                                 connection.server_listening_socket_,
                                                 active_cgi_processes_);
    }
}

void ServerManager::Respond500(Connection &connection) {
    DetachCGI(connection);
    // prepare to respond with 500
    connection.waiting_for_cgi_ = false;
    connection.body_done_ = true;
    connection.location_.SetReturnCode(FAILED_CGI);
    connection.location_.return_custom_message_ =
            "Connection closed out while no events were "
            "reported on the CGI's IO";
    Respond(connection);
}

/**
 * At this point there might be cgi connections on which events weren't
 * reported, because cgi process ended and / or closed the connection.
 * - If connection is waiting for CGI process to end - we'll try to read a  bit
 * from cgi_stdout_fd ("handle" event that didn't happen).
 *      - If read returns -1 - cgi process is still running and needs to be
 *      stopped.
 *      - If read returns 0 - cgi process is done, and we need to close the
 *      connection.
 */
void ServerManager::DetachCGI(Connection &connection) {
    char c;
    if (read(connection.cgi_stdout_fd_, &c, 1) == -1)
        kill(connection.cgi_pid_, SIGSTOP);
    if (connection.cgi_stdin_fd_ > 0)
        CloseCGIfd(connection.cgi_stdin_fd_);
    CloseCGIfd(connection.cgi_stdout_fd_);
    active_cgi_processes_--;
    closed_cgi_processes_++;
}

void ServerManager::CloseCGIfd(int terminated_cgi) {
	// delete cgi fd from epoll
	epoll_ctl(terminated_cgi, EPOLL_CTL_DEL, epoll_fd_, NULL);
	// close communication socket
	close(terminated_cgi);
	cgifd_to_cl_sock_.erase(terminated_cgi);
}

void ServerManager::HandleCGIEvent(int cgi_fd) {
	int             clients_socket = cgifd_to_cl_sock_.find(cgi_fd)->second;
	Connection      &connection = connections_[clients_socket];
	const Server    &server = FindServer(connection);

	if (connection.cgi_stdout_fd_ == cgi_fd) {
        // reading from cgi
		int status = server.HandleCGIinput(connection);
		if (status == CLIENT_CLOSED_CONNECTION_WHILE_CGI_SENDS_DATA) {
            return;
		} else if (status == NOT_ALL_DATA_READ_FROM_CGI) {
			return;
		} else if (status == ALL_READ_ALL_SENT) {
            DetachCGI(connection);
            CloseConnectionWithLogMessage(connection.connection_socket_,
                                          "CGI transmission ended");
		}
	} else if (connection.cgi_stdin_fd_ == cgi_fd) {
        // writing to cgi
		int status = server.HandleCGIoutput(connection);
		if (status == NOT_ALL_DATA_WRITTEN_TO_CGI) {
			return;
		} else if (status == CGI_CLOSED_INPUT_FD) {
            ThrowException("CGI_CLOSED_INPUT_FD");
		} else if (status == ALL_DATA_SENT_TO_CGI) {
            CloseCGIfd(connection.cgi_stdin_fd_);
            connection.cgi_stdin_fd_ = -1;
		}
	}
}
