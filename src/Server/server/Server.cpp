/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Server.cpp                                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:02:49 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include "Server.h"
#include "../request/RequestExceptions.h"

Server::Server(const ServerConfiguration &config) : AServer(config) {}

/**
 *  TODO MAX_CLIENTS
 */
void    Server::HandleEvents() {
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
            int client_sock = accept(GetSocket(), (struct sockaddr *) &client_addr,
                    &client_len);
            CheckRequest(client_sock, client_addr);
        }
        if (events[i].events & EPOLLIN && events[i].events & EPOLLOUT) {
                HandleRequest(fd);
        }
    }
}

void Server::HandleRequest(int client_sock) {
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

Server &Server::operator=(const Server &other) {
    if (this == &other)
        return *this;
    return *this;
}
