/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Aserver.cpp                                        :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 15:38:05 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "AServer.h"

AServer::AServer(const AServer &other)
        : config_(other.config_), socket_(other.socket_), epoll_fd_(other.epoll_fd_) {}

AServer::AServer(const ServerConfiguration &config)
        : config_(config), socket_(0), epoll_fd_(0) {}

AServer::~AServer() {}

const ServerConfiguration & AServer::GetConfig() const {
    return config_;
}

int AServer::GetSocket() const {
    return socket_;
}

int AServer::GetEpollFd() const {
    return epoll_fd_;
}

std::ostream &operator<<(std::ostream &os, const AServer &server) {
    os << "config_:\n" << server.config_ <<
       "socket_: " << server.socket_ <<
       "\nepoll_fd_: " << server.epoll_fd_ << std::endl;
    return os;
}

void AServer::Log(const std::string &msg, std::ostream &os) const {
    os << "Server " << config_.GetServerName();
    os << ":" << config_.GetPort() << " : " << msg << std::endl;
}
