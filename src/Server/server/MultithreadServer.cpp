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
#include "MultithreadServer.h"
#include "../request/RequestExceptions.h"

MultithreadServer::MultithreadServer(const ServerConfiguration &config,
                                     ThreadPool &pool)
: AServer(config), pool_(pool) {}

MultithreadServer::MultithreadServer(const AServer &server, ThreadPool &pool)
: AServer(server), pool_(pool) {}

MultithreadServer::MultithreadServer(const MultithreadServer &server)
: AServer(server), pool_(server.pool_) {}

void MultithreadServer::HandleRequest(int client_sock) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(),
                             GetConfig().GetPort());
    try {
        request.Init(client_sock);
        Log("Got client request:\n");
        std::cout << request << std::endl;
        response_location = ProcessRequest(request, client_sock);
        Log("Request processed");
    } catch (const HTTPVersionNotSupportedException &) {
        response_location.SetReturnCode(BAD_HTTP_VERSION);
    } catch (const ReadFromSocketFailedException &) {
        response_location.SetReturnCode(FAILED_IO);
    } catch (const ClientRequest::RequestException &) {
        response_location.SetReturnCode(BAD_REQUEST);
    }

    response.ComposeResponse(response_location);
    Log("Prepared response:\n");
    std::cout << response << std::endl;
    response.SendResponse(client_sock);
    Log("Response sent\n");
    close(client_sock);
}

void MultithreadServer::HandleEvents() {
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(GetEpollFd(), events, MAX_EVENTS, -1);
    if (nfds == -1) {
        // Handle epoll_wait error
        return;
    }
    for (int i = 0; i < nfds; ++i) {
        int fd = events[i].data.fd;
        if (fd == GetSocket()) {
            // New connection
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(GetSocket(),
                                     (struct sockaddr *) &client_addr,
                                             &client_len);
            CheckRequest(client_sock, client_addr);
        }
        if (events[i].events & EPOLLIN && events[i].events & EPOLLOUT) {
//            pthread_t worker;
//            void *arg = reinterpret_cast<void *>(fd);
//            pthread_create(&worker, NULL, &MultithreadServer::HandleRequest, arg);
        }
    }
}