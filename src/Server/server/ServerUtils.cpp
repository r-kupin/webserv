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
    Log(msg);
    connections_[client_sock] = Connection();
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_sock, NULL);
    close(client_sock);
}

void    Server::Log(const std::string &msg, std::ostream &os) const {
    os << config_.GetServerName() << ":" << config_.GetPort();
    os << " : " << msg << std::endl;
}

bool    Server::SetDescriptorNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

void Server::PrintEventInfo(int events, int fd, int i) {
    epoll_events_count_++;
    if (events == 5)
        epoll_in_out_count_++;
    if (events == EPOLLIN && fd == socket_ )
        epoll_connection_count_++;

    std::cout << "\n== returns " << epoll_returns_count_ <<
              " == events " << epoll_events_count_ <<
              " == connections " << epoll_connection_count_ <<
              " == IO " << epoll_in_out_count_ << "\n";

    std::cout << "nfd: " << i << "\n" << "fd: " << fd << "\n";
    if (events & EPOLLIN)
        std::cout << "EPOLLIN " << EPOLLIN << "\n";
    if (events & EPOLLOUT)
        std::cout << "EPOLLOUT " << EPOLLOUT << "\n";
    if (events & EPOLLERR)
        std::cout << "EPOLLERR " << EPOLLERR << "\n";
    std::cout << events << std::endl;
}
