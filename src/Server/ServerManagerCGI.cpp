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

/**
 * At this point there might be cgi connections on which events weren't
 * reported, because cgi process ended and / or closed the connection. In
 * this method we are looping across all connections:
 * - If connection is waiting for CGI process to end - we'll try to read a  bit
 * from cgi_stdout_fd ("handle" event that didn't happen).
 *      - If read returns -1 - cgi process is still running and might send us
 *      something.
 *      - If read returns 0 - cgi process is done, and we need to close the
 *      connection.
 * - If connection is NOT waiting for CGI process - it means that the limit
 * of processes was reached at the time of initialisation of this connection,
 * and if current amount of processes is less than MAX_CGI_PROCESSES we'll 
 * launch it 
 */
void ServerManager::CheckInactiveCGIs() {
	for (std::map<int, int>::iterator it = cgifd_to_cl_sock_.begin();
		 it != cgifd_to_cl_sock_.end(); ++it) {
		Connection &connection = connections_[it->second];
        if (connection.waiting_for_cgi_) {
            char c;
            if (read(connection.cgi_stdout_fd_, &c, 1) == -1)
                kill(connection.cgi_pid_, SIGSTOP);
            if (connection.cgi_stdin_fd_ > 0)
                CloseCGIfd(connection.cgi_stdin_fd_);
            CloseCGIfd(connection.cgi_stdout_fd_);
        } else {

        }
	}
    ThrowException("fini");
}

//void ServerManager::CloseCGIfd(int terminated_cgi) {
//    // delete cgi fd from epoll
//    epoll_ctl(terminated_cgi, EPOLL_CTL_DEL, epoll_fd_, NULL);
//    // close communication socket
//    close(terminated_cgi);
//    // remove mapping entry
//    CloseConnectionWithLogMessage(cgifd_to_cl_sock_[terminated_cgi],
//                                  "Cgi transmission ended.");
//    cgifd_to_cl_sock_.erase(terminated_cgi);
//    active_cgi_processes_--;
//}

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
//          kill(connection.cgi_pid_, SIGSTOP);
//			CloseCGIfd(connection.cgi_stdin_fd_);
//			CloseCGIfd(connection.cgi_stdout_fd_);
//			active_cgi_processes_--;
//			CloseConnectionWithLogMessage(clients_socket, "Clent died");
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
