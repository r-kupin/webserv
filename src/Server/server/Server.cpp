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
        socket_(other.socket_),
        epoll_fd_(other.epoll_fd_),
        epoll_returns_count_(0),
        epoll_events_count_(0),
        epoll_connection_count_(0),
        epoll_in_out_count_(0),
        connections_(MAX_CLIENTS) {}

Server::Server(const ServerConfiguration &config,
               const volatile bool &is_running_ref)
        : is_running_(is_running_ref),
        config_(config),
        socket_(0),
        epoll_fd_(0),
        epoll_returns_count_(0),
        epoll_events_count_(0),
        epoll_connection_count_(0),
        epoll_in_out_count_(0),
        connections_(MAX_CLIENTS) {}

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "config_:\n" << server.config_ <<
       "socket_: " << server.socket_ <<
       "\nepoll_fd_: " << server.epoll_fd_ << std::endl;
    return os;
}


