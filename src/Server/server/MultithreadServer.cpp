/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    MultithreadServer.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 11:51:56 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <sstream>
#include "MultithreadServer.h"
#include "../request/RequestExceptions.h"

MultithreadServer::MultithreadServer(const ServerConfiguration &config,
                                     ThreadPool &pool)
: AServer(config), pool_(pool), logs_buff_(1000, "") {}

MultithreadServer::MultithreadServer(const AServer &server, ThreadPool &pool)
: AServer(server), pool_(pool) {}

MultithreadServer::MultithreadServer(const MultithreadServer &server)
: AServer(server), pool_(server.pool_) {}

void MultithreadServer::HandleEvents() {
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(GetEpollFd(), events, MAX_EVENTS, -1);
    if (nfds == -1) {
        // Handle epoll_wait error
        return;
    }
    for (int i = 0; i < nfds; ++i) {
        int fd = events[i].data.fd;
        if (fd == GetEpollFd()) {
            // New connection
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(GetEpollFd(),
                                     (struct sockaddr *) &client_addr,
                                     &client_len);
            CheckRequest(client_sock, client_addr);
        }
        if (events[i].events & EPOLLIN && events[i].events & EPOLLOUT) {

        }
    }
}

void MultithreadServer::HandleRequest(int client_sock, std::ostream &os) {
    (void)client_sock;
    (void)os;
}

void* threadStart(void* arg) {
    HandlerFunctor* func = static_cast<HandlerFunctor*>(arg);
    std::string result = (*func)();
    delete func; // Clean up the allocated memory
    return new std::string(result);
}

HandlerFunctor::HandlerFunctor(int fd) : fd_(fd) {}

std::string HandlerFunctor::operator()() {
    return "hello";
}

