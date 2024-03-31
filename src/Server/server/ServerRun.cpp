/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerRun.cpp                                      :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 14:52:51 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <netinet/in.h>
#include <unistd.h>

#include "Server.h"

void    Server::Start() {
    if (!Init(0)) {
        Log("Server startup failed", log_file_);
        return;
    }
    Log("Server initialized successfully!", log_file_);
    log_file_ << *this << std::endl;
    Log("Server started", log_file_);
    while (is_running_)
        EventLoop();
    Cleanup();
    Log("Server stopped.", log_file_);
}

/**
 *  Server retrieves the reference to the current connection.
 *  It essentially serves to save the state of the connection, due to the
 * possibility of 1 request being sent in multiple events it allows us not to
 * lose data if on the stage of finishing of the data request being processed
 * is still incomplete and therefore response weren't yet sent.
 */
void Server::HandleEvents(int client_sock) {
    Connection      &connection = connections_[client_sock];

    while (is_running_) {
        if (!connection.url_headers_done_) {
            if (!ProcessHeaders(client_sock, connection))
                return;
        }
        if (connection.url_headers_done_ && !connection.body_done_) {
            if (!ProcessBody(client_sock, connection))
                return;
        }
        if (connection.body_done_) {
            Respond(client_sock, connection);
        }
    }
}

int Server::CheckRequest(int client_sock, int fd) {
    if (client_sock < 0) {
        Log("Error accepting connection!", log_file_);
    } else if (AddClientToEpoll(client_sock)) {
        // associate client's socket with server's listener
        connections_[client_sock] = Connection(is_running_, client_sock, fd);
        Log("Accepted client connection from socket " +
            Utils::NbrToString(client_sock), log_file_);
    } else {
        Log("Error adding client socket to epoll", log_file_);
        close(client_sock);
    }
    return client_sock;
}

/**
 * Add new socket created by accept to epoll instance
 */
bool Server::AddClientToEpoll(int client_sock) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = client_sock;
    if (SetDescriptorNonBlocking(client_sock))
        return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_sock, &event) != -1;
    Log("Can't set descriptor " + Utils::NbrToString(client_sock) +
        " to nonblocking mode.", log_file_);
    return false;
}