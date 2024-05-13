/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerRun.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 16:20:03 by  rokupin          #+#    #+#             */
/*                                                    ###   ########.fr       */
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
 * from cgi_fd ("handle" event that didn't happen).
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
    // no events were reported during epoll_wait timeout:
    // check all existing connections and close expired ones
    // 0 - no cgi connections
    // -1 - no terminated cgi connections
    // > 0 - terminated one
    int terminated_cgi = 0;
    for (std::map<int, Connection &>::iterator it = cgi_fd_to_conn_.begin();
         it != cgi_fd_to_conn_.end(); ++it) {
        // There are some CGI connections
        Connection connection = it->second;
        if (connection.waiting_for_cgi_) {
            if ((terminated_cgi = HandleCGIEvent(connection)) != -1) {
                // can't erase element from a map in wich we are iterating.
                // break and relaunch
                break;
            }
        } else if (active_cgi_processes_ < MAX_CGI_PROCESSES) {
            ReInvokeRequestProcessing(connection);
        }
    }
    if (terminated_cgi > 0) {
        // found map entry with a connection that ended it's communication with a
        // CGI process, and needs to be removed to avoid further attempts to
        // access the FD associated with a dead cgi process
        cgi_fd_to_conn_.erase(terminated_cgi);
        // Check the rest of them
        CheckInactiveCGIs();
    }
}

void ServerManager::ReInvokeRequestProcessing(Connection &connection) {
    const Server &server = FindServer(connection);
    connection.location_ = server.ProcessRequest(connection);
}

void ServerManager::HandleCGIEvent(int fd) {
    if (cgi_fd_to_conn_.find(fd) != cgi_fd_to_conn_.end()) {
        HandleCGIEvent(cgi_fd_to_conn_.find(fd)->second);
    }
}

int ServerManager::HandleCGIEvent(Connection &connection) {
    try {
        const Server &server = FindServer(connection);
        server.HandleCGIinput(connection);
    } catch (const EwouldblockEagainUpload &) {
        Log("Red all available data, but cgi transmission is incomplete. "
            "We'll come back later. Maybe.");
    } catch (...) {
        // delete cgi fd from epoll
        epoll_ctl(connection.cgi_fd_, EPOLL_CTL_DEL, epoll_fd_, NULL);
        // close communication socket
        close(connection.cgi_fd_);
        connections_[connection.connection_socket_] =
                Connection(is_running_, connection.connection_socket_,
                           connection.server_listening_socket_, connection.active_cgis_);
        return connection.cgi_fd_;
    }
    return -1;
}
