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

#include <fcntl.h>
#include <csignal>
#include "ServerManager.h"

/**
 * Ask each server does it listens to this particular socket or not
 */
bool            ServerManager::IsListeningSocketFd(int socket) const {
    for (v_servers ::const_iterator it = servers_.begin();
            it != servers_.end(); ++it) {
        if (it->ListensTo(socket))
            return true;
    }
    return false;
}

const Server &ServerManager::FindServerByListeningSocket(int socket) const {
    for (v_servers ::const_iterator it = servers_.begin();
         it != servers_.end(); ++it) {
        if (it->ListensTo(socket))
            return *it;
    }
    throw std::exception(); // this will never happen
}

void            ServerManager::ThrowException(const std::string &msg) const {
    Log(msg);
    throw std::exception();
}

void ServerManager::CloseConnectionWithLogMessage(int socket, const std::string &msg) {
    Log(msg);
    connections_[socket] = Connection(is_running_);
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket, NULL);
    close(socket);
}

void ServerManager::Log(const std::string &msg) const {
    std::cout << "[ " << Utils::TimeNow() - startup_time_ << " ] ";
    std::cout << msg << std::endl;
}

void ServerManager::PrintEventInfo(int events, int fd, int i) {
    epoll_events_count_++;
    if (events == 5)
        epoll_in_out_count_++;
    if (events == EPOLLIN && IsListeningSocketFd(fd))
        epoll_connection_count_++;

    std::cout << /*"\n== returns " << epoll_returns_count_ <<*/
              "\n== events " << epoll_events_count_ <<
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

bool    ServerManager::SetDescriptorNonBlocking(int sockfd) const {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        Log("fcntl get flags operation failed");
        return false;
    }
    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

void ServerManager::Cleanup() {
    for (v_servers::iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->Cleanup(epoll_fd_);
    close(epoll_fd_);
}