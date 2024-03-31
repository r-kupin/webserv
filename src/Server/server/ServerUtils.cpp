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

#include <sys/fcntl.h>
#include <csignal>
#include "Server.h"

void Server::CloseConnectionWithLogMessage(int client_sock, const std::string &msg) {
    Log(msg, log_file_);
    connections_[client_sock] = Connection(is_running_);
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_sock, NULL);
    close(client_sock);
}

void Server::ThrowException(const std::string &msg, std::ostream &os) const {
    Log(msg, os);
    throw ServerException();
}

void    Server::Log(const std::string &msg, std::ostream &os) const {
    os << "[ " << Utils::TimeNow() - startup_time_ << " ] ";
    os << msg << std::endl;
}

bool    Server::SetDescriptorNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        Log("fcntl get flags operation failed", log_file_);
        return false;
    }
    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

void Server::Cleanup() {
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, epoll_fd_, NULL);
    close(epoll_fd_);
    for (m_int_str::iterator it = srv_sock_to_address_.begin();
         it != srv_sock_to_address_.end(); ++it) {
        close(it->first);
    }
}


