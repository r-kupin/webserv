/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    AServerRun.cpp                                     :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 14:52:51 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "AServer.h"

#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/fcntl.h>

/**
 *  TODO MAX_CLIENTS
 */
void AServer::Start() {
    Init();
    Log("Server initialized successfully..\n");
    std::cout << *this << std::endl;
    Start(config_.GetPort());
}

void AServer::Start(int port) {
    if (epoll_fd_ > 0) {
        Log("started server at " + Utils::NbrToString(port) + " port");
        while (true) {HandleEvents();}
    } else {
        Log("It seems like " + Utils::NbrToString(port) +
            " port is already in use. Aborting.");
    }
    close(socket_);
}

bool    set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

int AServer::CheckRequest(int client_sock, const sockaddr_in &client_addr) {
    if (client_sock < 0) {
        Log("Error accepting connection!");
    } else if (AddClientToEpoll(client_sock, epoll_fd_)) {
        Log("Accepted client connection from " +
            Utils::NbrToString(client_addr.sin_addr.s_addr) + "\n");
    } else {
        Log("Error adding client socket to epoll of set nonblocking");
        close(client_sock);
    }
    return client_sock;
}

/**
 *  TODO MAX_CLIENTS
 */
void    AServer::HandleEvents() {
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
    if (nfds == -1) {
        // Handle epoll_wait error
        return;
    }
    for (int i = 0; i < nfds; ++i) {
        int fd = events[i].data.fd;
        if (fd == socket_) {
            // New connection
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(socket_, (struct sockaddr *) &client_addr,
                                     &client_len);
            CheckRequest(client_sock, client_addr);
        } else if (events[i].events & EPOLLIN && events[i].events & EPOLLOUT) {
            HandleRequest(fd);
        }
    }
}