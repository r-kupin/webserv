/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManagerUtils.cpp                             :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/31 16:36:08 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ServerManager.h"


bool            ServerManager::IsSocketFd(int fd) const {
    return srv_sock_to_address_.find(fd) != srv_sock_to_address_.end();
}

void            ServerManager::ThrowException(const std::string &msg) const {
    Log(msg);
    throw std::exception();
}

void ServerManager::Log(const std::string &msg) const {
    std::cout << "[ " << Utils::TimeNow() - startup_time_ << " ] ";
    std::cout << msg << std::endl;
}

void ServerManager::PrintEventInfo(int events, int fd, int i) {
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
