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
#include <sys/time.h>
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
    for (std::map<int, std::string>::iterator it = socket_to_address_.begin();
        it != socket_to_address_.end(); ++it) {
        close(it->first);
    }
}

bool Server::IsSocketFd(int fd) const {
    return socket_to_address_.find(fd) == socket_to_address_.end();
}

void Server::PrintEventInfo(int events, int fd, int i) {
    epoll_events_count_++;
    if (events == 5)
        epoll_in_out_count_++;
    if (events == EPOLLIN && IsSocketFd(fd))
        epoll_connection_count_++;

    log_file_ << /*"\n== returns " << epoll_returns_count_ <<*/
              "\n== events " << epoll_events_count_ <<
              " == connections " << epoll_connection_count_ <<
              " == IO " << epoll_in_out_count_ << "\n";

    log_file_ << "nfd: " << i << "\n" << "fd: " << fd << "\n";
    if (events & EPOLLIN)
        log_file_ << "EPOLLIN " << EPOLLIN << "\n";
    if (events & EPOLLOUT)
        log_file_ << "EPOLLOUT " << EPOLLOUT << "\n";
    if (events & EPOLLERR)
        log_file_ << "EPOLLERR " << EPOLLERR << "\n";
    log_file_ << events << std::endl;
}
