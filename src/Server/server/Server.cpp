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

#include "Server.h"

Server::Server(const Server &other)
        : config_(other.config_),
          socket_(other.socket_),
          epoll_fd_(other.epoll_fd_),
          epoll_returns_count_(0),
          epoll_events_count_(0),
          epoll_connection_count_(0),
          epoll_in_out_count_(0),
          connections_(MAX_CLIENTS) {}

Server::Server(const ServerConfiguration &config)
        : config_(config),
          socket_(0),
          epoll_fd_(0),
          epoll_returns_count_(0),
          epoll_events_count_(0),
          epoll_connection_count_(0),
          epoll_in_out_count_(0),
          connections_(MAX_CLIENTS) {}

Server &Server::operator=(const Server &other) {
    if (this == &other)
        return *this;
    return *this;
}

const ServerConfiguration & Server::GetConfig() const {
    return config_;
}

int Server::GetSocket() const {
    return socket_;
}

int Server::GetEpollFd() const {
    return epoll_fd_;
}

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "config_:\n" << server.config_ <<
       "socket_: " << server.socket_ <<
       "\nepoll_fd_: " << server.epoll_fd_ << std::endl;
    return os;
}

void Server::Log(const std::string &msg, std::ostream &os) const {
    os << config_.GetServerName() << ":" << config_.GetPort();
    os << " : " << msg << std::endl;
}



