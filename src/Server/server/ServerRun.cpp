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

#include "Server.h"
#include "../connection/request/RequestExceptions.h"
#include "ServerExceptions.h"

#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <csignal>

/**
 *  TODO MAX_CLIENTS
 */
void Server::Start() {
    Init();
    Log("Server initialized successfully..\n");
    std::cout << *this << std::endl;
    if (epoll_fd_ > 0) {
        Log("started server at " + Utils::NbrToString(config_.GetPort()) + " port");
        while (is_running_) { HandleEvents(); }
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, epoll_fd_, NULL);
        close(epoll_fd_);
        Log("stpopped");
    } else {
        Log("It seems like " + Utils::NbrToString(config_.GetPort()) +
            " port is already in use. Aborting.");
    }
    close(socket_);
}

void Server::Stop(int signal) {
    if (signal == SIGINT) {
        std::cout << "stopping servers" << std::endl;
        is_running_ = false;
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

int Server::CheckRequest(int client_sock) {
    if (client_sock < 0) {
        Log("Error accepting connection!");
    } else if (AddClientToEpoll(client_sock, epoll_fd_)) {
        Log("Accepted client connection from socket " +
            Utils::NbrToString(client_sock));
    } else {
        Log("Error adding client socket to epoll");
        close(client_sock);
    }
    return client_sock;
}

void    print_event_info(int events,
                         int fd,
                         int socket_,
                         int i,
                         int epoll_returns_count_,
                         int &epoll_events_count_,
                         int &epoll_in_out_count_,
                         int &epoll_connection_count_) {
    epoll_events_count_++;
    if (events == 5)
        epoll_in_out_count_++;
    if (events == EPOLLIN && fd == socket_ )
        epoll_connection_count_++;

    std::cout << "\n== returns " << epoll_returns_count_ <<
              " == events " << epoll_events_count_ <<
              " == connections " << epoll_connection_count_ <<
              " == IO " << epoll_in_out_count_ << "\n";

    std::cout << "nfd: " << i << "\n" << "fd: " << fd << "\n";
    if (events & EPOLLIN)
        std::cout << "EPOLLIN " << EPOLLIN << "\n";
    if (events & EPOLLOUT)
        std::cout << "EPOLLOUT " << EPOLLOUT << "\n";
    if (events & EPOLLERR)
        std::cout << "EPOLLERR " << EPOLLERR << "\n";
    std::cout << events << std::endl;
}

/**
 *  TODO MAX_CLIENTS
 */
void    Server::HandleEvents() {
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
    if (is_running_) {
        epoll_returns_count_++;
        if (nfds == -1) {
            // Handle epoll_wait error
            return;
        }
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            print_event_info(events[i].events, fd, socket_, i,
                             epoll_returns_count_, epoll_events_count_,
                             epoll_in_out_count_, epoll_connection_count_);
            if (fd == socket_) {
                // New connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_sock = accept(socket_, (struct sockaddr *) &client_addr,
                                         &client_len);
                CheckRequest(client_sock);
            } else if (events[i].events & EPOLLIN && events[i].events & EPOLLOUT) {
                HandleRequest(fd);
            }
        }
    }
}

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