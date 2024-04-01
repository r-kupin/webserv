/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerUtils.cpp                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/22 13:53:34 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include "Server.h"

const std::string &Server::GetAddress(int socket) const {
    return srv_sock_to_address_.find(socket)->second;
}

void Server::ThrowException(const std::string &msg, std::ostream &os) const {
    Log(msg, os);
    throw ServerException();
}

void    Server::Log(const std::string &msg, std::ostream &os) const {
    os << "[ " << Utils::TimeNow() - startup_time_ << " ] ";
    os << msg << std::endl;
}

void Server::Cleanup(int epoll_fd) {
    for (m_int_str::iterator it = srv_sock_to_address_.begin();
         it != srv_sock_to_address_.end(); ++it) {
        // Remove each listening socket from epoll_fd instance
        epoll_ctl(it->first, EPOLL_CTL_DEL, epoll_fd, NULL);
        // close socket
        close(it->first);
    }
}

bool Server::ListensTo(int socket) const {
    return srv_sock_to_address_.find(socket) != srv_sock_to_address_.end();
}
