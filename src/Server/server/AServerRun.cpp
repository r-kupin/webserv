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
#include <csignal>

bool    set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

bool    set_timeout(int sockfd) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10;
    return (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) != -1);
}

/**
 *  TODO MAX_CLIENTS
 */
void AServer::Start() {
    Init();
    /*set_non_blocking(epoll_fd_);*/
    Log("Server initialized successfully..\n");
    std::cout << *this << std::endl;
    Start(config_.GetPort());
}

void AServer::Start(int port) {
    if (epoll_fd_ > 0) {
        Log("started server at " + Utils::NbrToString(port) + " port");
        while (is_running_) {HandleEvents();}
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, epoll_fd_, NULL);
        close(epoll_fd_);
        Log("stpopped");
    } else {
        Log("It seems like " + Utils::NbrToString(port) +
            " port is already in use. Aborting.");
    }
    close(socket_);
}

void AServer::Stop(int signal) {
    if (signal == SIGINT) {
        std::cout << "stopping servers" << std::endl;
        is_running_ = false;
    }
}

int AServer::CheckRequest(int client_sock, const sockaddr_in &client_addr) {
    if (client_sock < 0) {
        Log("Error accepting connection!");
    } else if (AddClientToEpoll(client_sock, epoll_fd_)
            && set_timeout(client_sock)) {
        Log("Accepted client connection from " +
            Utils::NbrToString(client_addr.sin_addr.s_addr));
    } else {
        Log("Error adding client socket to epoll");
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
    if (is_running_) {
        epoll_returns_count_++;
        std::cout << "\n========" << epoll_returns_count_ << "========" << std::endl;
        if (nfds == -1) {
            // Handle epoll_wait error
            return;
        }
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            std::cout << "nfd: " << i << "\n" << "fd: " << fd << std::endl;
            if (events[i].events & EPOLLIN)
                std::cout << "EPOLLIN " << EPOLLIN << std::endl;
            if (events[i].events & EPOLLOUT)
                std::cout << "EPOLLOUT " << EPOLLOUT << std::endl;
            if (events[i].events & EPOLLERR)
                std::cout << "EPOLLERR " << EPOLLERR << std::endl;
            std::cout << events[i].events << std::endl;
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
}