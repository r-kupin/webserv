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

void ServerManager::PrintEventInfo(int events, int fd, int i) {
    epoll_events_count_++;
    if (events == 5)
        epoll_in_out_count_++;
    if (events == EPOLLIN && IsSocketFd(fd))
        epoll_connection_count_++;

    log_file_ << /*"\n== returns " << epoll_returns_count_ <<*/
              "\n== events " << epoll_events_count_ <<
              " == connections " << epoll_connection_count_ <<
              " == IO " << epoll_in_out_count_ << "\n";

    log_file_ << "nfd: " << i << "\n" << "fd: " << fd << "\n";
    if (events & EPOLLIN)
        log_file_ << "EPOLLIN " << EPOLLIN << "\n";
    if (events & EPOLLOUT)
        log_file_ << "EPOLLOUT " << EPOLLOUT << "\n";
    if (events & EPOLLERR)
        log_file_ << "EPOLLERR " << EPOLLERR << "\n";
    log_file_ << events << std::endl;
}
