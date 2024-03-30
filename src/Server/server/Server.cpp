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
        : is_running_(other.is_running_),
          config_(other.config_),
          files_uploaded_(other.files_uploaded_),
          socket_to_address_(other.socket_to_address_),
          epoll_fd_(other.epoll_fd_),
          epoll_returns_count_(0),
          epoll_events_count_(0),
          epoll_connection_count_(0),
          epoll_in_out_count_(0),
          connections_(CreateConnections(MAX_CLIENTS, is_running_)),
          startup_time_(other.startup_time_) {}

Server::Server(const ServerConfiguration &config,
               const volatile bool &is_running_ref)
        : is_running_(is_running_ref),
          config_(config),
          files_uploaded_(0),
          epoll_fd_(0),
          epoll_returns_count_(0),
          epoll_events_count_(0),
          epoll_connection_count_(0),
          epoll_in_out_count_(0),
          connections_(CreateConnections(MAX_CLIENTS, is_running_ref)),
          startup_time_(Utils::TimeNow()) {}

v_conn Server::CreateConnections(int n, const volatile bool &running) {
    std::vector<Connection> connections;

    for (int i = 0; i < n; ++i) {
        connections.push_back(Connection(running));
    }
    return connections;
}

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "config:\n" << server.config_;
    os << "\naddresses:\n";

    for (std::map<int, std::string>::const_iterator it = server.socket_to_address_.begin();
         it != server.socket_to_address_.end();
         ++it) {
        os << "\t" << it->second << "\n";
    }
    return os;
}


