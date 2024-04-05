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
#include <csignal>

Server::Server(const Server &other)
        : is_running_(other.is_running_),
          config_(other.config_),
          srv_sock_to_address_(other.srv_sock_to_address_) {}

Server::Server(const ServerConfiguration &config, v_c_b &is_running_ref, int epoll_fd)
        : is_running_(is_running_ref),
          config_(config) { Init(epoll_fd); }

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "config:\n" << server.config_;
    os << "\naddresses:\n";

    for (m_int_str::const_iterator it = server.srv_sock_to_address_.begin();
         it != server.srv_sock_to_address_.end(); ++it) {
        os << "\t" << it->second << "\n";
    }
    return os;
}

long Server::GetConnectionTimeout() const {
    return config_.GetKeepaliveTimeout();
}

const std::string &Server::GetAddress(int socket) const {
    return srv_sock_to_address_.find(socket)->second;
}

const m_int_str &Server::GetSrvSockToAddress() const {
    return srv_sock_to_address_;
}

void Server::ThrowException(const std::string &msg) const {
    Log(msg);
    throw ServerException();
}

void Server::ThrowException(const std::string &msg, int listen_sock) const {
    Log(msg, listen_sock);
    throw ServerException();
}

void Server::Log(const std::string &msg) const {
    std::cout << "[ " << Utils::Get().TimeElapsed() << " ] Server : ";
    std::cout << msg << std::endl;
}

void Server::Log(const std::string &msg, int listen_sock) const {
    std::cout << "[ " << Utils::Get().TimeElapsed() << " ] ";
    std::cout << srv_sock_to_address_.find(listen_sock)->second << " : ";
    std::cout << msg << std::endl;
}

void Server::Cleanup(int epoll_fd) {
    for (m_int_str::iterator it = srv_sock_to_address_.begin();
         it != srv_sock_to_address_.end(); ++it) {
        // Remove each listening socket from epoll_fd instance
        epoll_ctl(it->first, EPOLL_CTL_DEL, epoll_fd, NULL);
        // close socket, no need to shutdown() here
        close(it->first);
    }
}

bool Server::ListensTo(int socket) const {
    return srv_sock_to_address_.find(socket) != srv_sock_to_address_.end();
}
