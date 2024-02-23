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
#include <cstring>
#include "Server.h"
#include "../request/RequestExceptions.h"
#include "ServerExceptions.h"

Server::Server(const ServerConfiguration &config) : AServer(config) {}

void Server::HandleRequest(int client_sock) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(),
                             GetConfig().GetPort());
    try {
        request.Init(client_sock);
        Log("Got client request:\n", std::cout);
        std::cout << request << std::endl;
        response_location = ProcessRequest(request, std::cout, client_sock);
        Log("Request processed", std::cout);
    } catch (const HTTPVersionNotSupportedException &) {
        response_location.SetReturnCode(BAD_HTTP_VERSION);
    } catch (const ReadFromSocketFailedException &) {
        response_location.SetReturnCode(FAILED_IO);
    } catch (const ClientRequest::RequestException &) {
        response_location.SetReturnCode(BAD_REQUEST);
    }

    response.ComposeResponse(response_location);
    Log("Prepared response:\n", std::cout);
    std::cout << response << std::endl;
    response.SendResponse(client_sock);
    Log("Response sent\n", std::cout);
    close(client_sock);
}

bool Server::AddClientToEpoll(int client_sock, int epoll_fd) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = client_sock;
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) != -1;
}

void Server::AddEpollInstance() {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = GetSocket();
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(GetEpollFd(), EPOLL_CTL_ADD, GetSocket(), &event) < 0)
        throw EpollAddFailed();
}

Server &Server::operator=(const Server &other) {
    if (this == &other)
        return *this;
    return *this;
}
