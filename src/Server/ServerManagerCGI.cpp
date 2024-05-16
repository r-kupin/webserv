/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerCGI.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 16:20:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/15 20:10:55 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include "ServerManager.h"
#include "server/ServerExceptions.h"

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
//    // no events were reported during epoll_wait timeout:
//    // check all existing connections and close expired ones
//    // 0 - no cgi connections
//    // -1 - no terminated cgi connections
//    // > 0 - terminated one
//    int terminated_cgi = 0;
//    for (std::map<int, int>::iterator it = cgifd_to_cl_sock_.begin();
//         it != cgifd_to_cl_sock_.end(); ++it) {
//        std::cout << "cgi fd: " << it->first << " socket: " << it->second <<
//        std::endl;
//        // There are some CGI connections
//        Connection &connection = connections_[it->second];
//        if (connection.waiting_for_cgi_) {
//            if ((terminated_cgi = HandleCGIEvent(connection, 0)) != -1) {
//                // can't erase element from a map in wich we are iterating.
//                // break and relaunch
//                break;
//            }
//        } else if (active_cgi_processes_ < MAX_CGI_PROCESSES) {
//            ReInvokeRequestProcessing(connection);
//        }
//    }
//    if (terminated_cgi > 0) {
//        // found map entry with a connection that ended it's communication with a
//        // CGI process, and needs to be removed to avoid further attempts to
//        // access the FD associated with a dead cgi process
//        HandleClosedCGIfd(terminated_cgi);
//        // Check the rest of them
//        CheckInactiveCGIs();
//    }
}

//void ServerManager::HandleClosedCGIfd(int terminated_cgi) {
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

void ServerManager::HandleClosedCGIfd(int terminated_cgi) {
    // delete cgi fd from epoll
    epoll_ctl(terminated_cgi, EPOLL_CTL_DEL, epoll_fd_, NULL);
    // close communication socket
    close(terminated_cgi);
    cgifd_to_cl_sock_.erase(terminated_cgi);
}


void ServerManager::ReInvokeRequestProcessing(Connection &connection) {
	const Server &server = FindServer(connection);
	connection.location_ = server.ProcessRequest(connection);
}

void ServerManager::HandleCGIEvent(int cgi_fd) {
    int             clients_socket = cgifd_to_cl_sock_.find(cgi_fd)->second;
    Connection      &connection = connections_[clients_socket];
    const Server    &server = FindServer(connection);

    if (connection.cgi_stdout_fd_ == cgi_fd) {// reading from cgi
        int status = server.HandleCGIinput(connection);
        if (status == CLIENT_CLOSED_CONNECTION_WHILE_CGI_SENDS_DATA) {
            kill(connection.cgi_pid_, SIGSTOP);
            HandleClosedCGIfd(connection.cgi_stdin_fd_);
            HandleClosedCGIfd(connection.cgi_stdout_fd_);
            active_cgi_processes_--;
        } else if (status == NOT_ALL_DATA_READ_FROM_CGI) {
            return;
        } else if (status == ALL_READ_ALL_SENT) {
            HandleClosedCGIfd(connection.cgi_stdin_fd_);
            HandleClosedCGIfd(connection.cgi_stdout_fd_);
            active_cgi_processes_--;
            close(connection.connection_socket_);
            // request answered. reset connection.
            connections_[connection.connection_socket_] = Connection(
                    is_running_, connection.connection_socket_,
                    connection.server_listening_socket_,
                    connection.active_cgis_);
        }
    } else if (connection.cgi_stdin_fd_ == cgi_fd) {
        int status = server.HandleCGIoutput(connection);
        if (status == NOT_ALL_DATA_WRITTEN_TO_CGI) {
            return;
        } else if (status == CGI_CLOSED_INPUT_FD) {
            HandleClosedCGIfd(cgi_fd);
        } else if (status == ALL_DATA_SENT_TO_CGI) {
            HandleClosedCGIfd(cgi_fd);
        }
    }
}
