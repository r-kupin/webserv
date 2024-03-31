/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManagerRun.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/31 16:20:36 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ServerManager.h"

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
 * append the response.
 */
void    ServerManager::EventLoop() {
    epoll_event events[MAX_EVENTS];
    // change timeout from -1 to be able to shut down server with ^C
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
    if (is_running_) {
        epoll_returns_count_++;
        if (nfds == -1) {
            ThrowException("Epoll wait failed. Shutting down.");
        } else {
            for (int i = 0; i < nfds; ++i) {
                int fd = events[i].data.fd;
                PrintEventInfo(events[i].events, fd, i);
                if (IsSocketFd(fd)) {
                    // New connection: fd == server's listening socket
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_sock = accept(fd,
                                             (struct sockaddr *) &client_addr,
                                             &client_len);
                    CheckRequest(client_sock, fd);
                } else if (events[i].events & EPOLLIN &&
                           events[i].events & EPOLLOUT) {
                    // New data on existing connection:
                    // fd == server's "end" of the pipe to communicate with client
                    HandleEvents(fd);
                }
            }
        }
    }
}

int ServerManager::CheckRequest(int client_sock, int fd) {
    if (client_sock < 0) {
        Log("Error accepting connection!");
    } else if (AddClientToEpoll(client_sock)) {
        // associate client's socket with server's listener
        connections_[client_sock] = Connection(is_running_, client_sock, fd);
        Log("Accepted client connection from socket " + Utils::NbrToString(client_sock));
    } else {
        Log("Error adding client socket to epoll");
        close(client_sock);
    }
    return client_sock;
}

/**
 * Add new socket created by accept to epoll instance
 */
bool ServerManager::AddClientToEpoll(int client_sock) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = client_sock;
    if (SetDescriptorNonBlocking(client_sock))
        return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_sock, &event) != -1;
    Log("Can't set descriptor " + Utils::NbrToString(client_sock) +
        " to nonblocking mode.");
    return false;
}