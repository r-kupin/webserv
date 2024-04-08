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
#include "../ServerManager.h"
#include <csignal>

Server::Server(const Server &other)
        : is_running_(other.is_running_),
          config_(other.config_),
          sock_to_host_(other.sock_to_host_) {}

Server::Server(const ServerConfiguration &config, v_c_b &is_running_ref,
               const std::map<Host, int> &all_open_sockets)
        : is_running_(is_running_ref),
          config_(config) {
    // iterate open sockets ServManager and match against ones defined in config
    const s_hosts &hosts_current_server = config.GetHosts();
    for (s_hosts::const_iterator it = hosts_current_server.begin();
         it != hosts_current_server.end(); ++it) {
        const m_host_int::const_iterator &found = all_open_sockets.find(*it);
        if (found != all_open_sockets.end() ) {
            // add opened socket to server's socket-to-address map
            sock_to_host_.insert(std::make_pair(found->second, found->first));
        }
    }
}

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "server:" << "\n";
    for (std::map<int, Host>::const_iterator it = server.sock_to_host_.begin();
            it != server.sock_to_host_.end(); ++it) {
        os << "\t" << it->second.host_ << ":" << it->second.port_;
        os << " listens on " << it->first << " socket\n";
    }
    os << "\n" << server.config_ << "\n";
    return os;
}

long Server::GetConnectionTimeout() const {
    return config_.GetKeepaliveTimeout();
}

std::string Server::GetAddress(int socket) const {
    const Host &host = sock_to_host_.find(socket)->second;
    return host.host_ + ":" + Utils::NbrToString(host.port_);
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
    std::cout << GetAddress(listen_sock) << " : ";
    std::cout << msg << std::endl;
}

void Server::Cleanup(int epoll_fd) {
    (void) epoll_fd;
//    for (m_int_str::iterator it = srv_sock_to_ipv4_.begin();
//         it != srv_sock_to_ipv4_.end(); ++it) {
//        // Remove each listening socket from epoll_fd instance
//        epoll_ctl(it->first, EPOLL_CTL_DEL, epoll_fd, NULL);
//        // close socket, no need to shutdown() here
//        close(it->first);
//    }
}

bool Server::ListensTo(int socket) const {
    return sock_to_host_.find(socket) != sock_to_host_.end();
}
