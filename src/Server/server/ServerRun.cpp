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
#include <csignal>

#include "Server.h"

void Server::Start() {
    Init();
    Log("Server initialized successfully!", log_file_);
    log_file_ << *this << std::endl;
    if (epoll_fd_ > 0) {
        Log("Started server at " +
            Utils::NbrToString(config_.GetPort()) + " port.", log_file_);
        while (is_running_)
            EventLoop();
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, epoll_fd_, NULL);
        close(epoll_fd_);
        Log("Server stopped.", log_file_);
        close(socket_);
    } else {
        close(socket_);
        ThrowException("It seems like " +
                        Utils::NbrToString(config_.GetPort()) +
                        " port is already in use. Aborting.", log_file_);
    }
}

/**
 *  Server's main loop
 *  Based around blocking call epoll_wait() which blocks on server's listening
 * socket socket_ while client wouldn't connect to the server.
 *  If that would happen - epoll_wait will return an event with
 * event.data.fd == socket_ and event.events == EPOLLIN. In this case server
 * accepts connection and adds new socket to the epoll instance, accessible
 * by epoll_fd_.
 *  Otherwise - epoll_wait will return cause to one or more of the already
 * accepted sockets send some data (request) to the server. In this case
 * event.data.fd == already-accepted-socket-fd and
 * event.events & EPOLLIN && event.events & EPOLLOUT. In this case server
 * accepts needs to read incoming data, create the Request, process it and
 * aend the response.
 */
void    Server::EventLoop() {
    epoll_event events[MAX_EVENTS];
    // change timeout from -1 to be able to shut down server with ^C
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
    if (is_running_) {
        epoll_returns_count_++;
        if (nfds == -1) {
            ThrowException("Epoll wait failed. Shutting down.", log_file_);
        } else {
            for (int i = 0; i < nfds; ++i) {
                int fd = events[i].data.fd;
                PrintEventInfo(events[i].events, fd, i);
                if (fd == socket_) {
                    // New connection
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_sock = accept(socket_,
                                             (struct sockaddr *) &client_addr,
                                             &client_len);
                    CheckRequest(client_sock);
                } else if (events[i].events & EPOLLIN && events[i].events & EPOLLOUT) {
                    HandleEvents(fd);
                }
            }
        }
    }
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

int Server::CheckRequest(int client_sock) {
    if (client_sock < 0) {
        Log("Error accepting connection!", log_file_);
    } else if (AddClientToEpoll(client_sock)) {
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