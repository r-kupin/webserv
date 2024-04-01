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
          srv_sock_to_address_(other.srv_sock_to_address_),
          startup_time_(other.startup_time_) {}

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


