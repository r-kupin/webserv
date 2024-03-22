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
#include <fcntl.h>
#include "Server.h"
#include "../connection/request/RequestExceptions.h"
#include "ServerExceptions.h"

Server::Server(const ServerConfiguration &config)
    : AServer(config), connections_(MAX_CLIENTS) {}

void Server::HandleRequest(int client_sock) {
    Connection      &connection = connections_[client_sock];

    while (is_running_) {
        if (!connection.url_headers_done_) {
            try {
                connection.request_.Init(client_sock);
                Log("Got client request:\n", std::cout);
                std::cout << connection.request_ << std::endl;
                connection.url_headers_done_ = true;
            } catch (const ZeroRead &e) {
                // socket is closed on the client's side. Remove connection
                Log("Done with all available request data, response already sent");
                connections_[client_sock] = Connection();
                epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
                close(client_sock);
                return;
            } catch (const EwouldblockEagain &e) {
                // socket is still active, but no data is available
                Log("Red all available data, but request is incomplete. "
                    "We'll come back later. Maybe.");
                return;
            } catch (const ReadFromSocketFailedException &) {
                // Probably, client wouldn't even be able to read our response
                // so just shut down this connection
                Log("Read operation failed");
                connections_[client_sock] = Connection();
                epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
                close(client_sock);
                return;
            } catch (const ClientRequest::RequestException &) {
                // notify client about it
                Log("Request misconfigured");
                connection.location_.SetReturnCode(BAD_REQUEST);
            }
        }
        if (connection.url_headers_done_ && !connection.body_done_) {
            try {
                connection.location_ = ProcessRequest(connection.request_,
                                                      std::cout, client_sock);
                Log("Request processed", std::cout);
                connection.body_done_ = true;
            } catch (const ZeroRead &) {
                Log("Client closed connection while we were reading curl file "
                    "metadata");
                connections_[client_sock] = Connection();
                epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
                close(client_sock);
                return;
            } catch (const ZeroReadUpload &) {
                Log("Client closed connection while we were uploading file");
                connections_[client_sock] = Connection();
                epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
                close(client_sock);
                return;
            } catch (const SendContinueFailedException &) {
                Log("Can't send 100 Continue. "
                    "Seems like client won't receive our response as well");
                break;
            } catch (const EwouldblockEagain &) {
                return;
            } catch (const EwouldblockEagainUpload &) {
                return;
            } catch (const ReadFromSocketFailedException &) {
                Log("Read operation failed");
                connections_[client_sock] = Connection();
                epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
                close(client_sock);
                return;
            } catch (const ClientRequest::RequestException &) {
                Log("Request misconfigured");
                connection.location_.SetReturnCode(BAD_REQUEST);
            }
        }
        if (connection.body_done_) {
            ServerResponse response(GetConfig().GetServerName(),
                                    GetConfig().GetPort());
            response.ComposeResponse(connection.location_);
            Log("Prepared response:\n", std::cout);
            std::cout << response << std::endl;
            response.SendResponse(client_sock);
            Log("Response sent\n", std::cout);
            connections_[client_sock] = Connection();
        }
    }
}

bool    set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

bool Server::AddClientToEpoll(int client_sock, int epoll_fd) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = client_sock;
    set_non_blocking(client_sock);
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
