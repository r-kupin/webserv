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
#include <cstring>

#include "ServerManager.h"

/**
 * Ask each server does it listens to this particular socket or not.
 */
bool            ServerManager::IsListeningSocketFd(int socket) const {
    for (v_servers::const_iterator it = servers_.begin();
            it != servers_.end(); ++it) {
        if (it->ListensTo(socket))
            return true;
    }
    return false;
}

/**
 * Method finds server that should handle request.
 * Request should bu processed by a server that:
 *  1. listens on the socket, on which request was reported
 *  2. has defined "server_name" that corresponds to the request's value of the
 *  "Host" header
 * If no server's "server_name" corresponds to the request's value of the "Host"
 * header - return the first server that listens on this socket defined in .conf
 * file
 */
const Server &ServerManager::FindServer(const Connection &connection) const {
    v_servers::const_iterator last_listening_server = servers_.end();

    // in fact, we iterate from the last to the first server defined in config
    for (v_servers::const_iterator it = servers_.begin();
        it != servers_.end(); ++it) {
        if (it->ListensTo(connection.server_listening_socket_)) {
            if (last_listening_server == servers_.end() ||
                !last_listening_server->HasServerName(connection.address_)) {
                last_listening_server = it;
            }
        }
    }
    return *last_listening_server;
}

void            ServerManager::ThrowException(const std::string &msg) const {
    Log(msg);
    throw ServerManagerException();
}

void ServerManager::CloseConnectionWithLogMessage(int socket,
                                                  const std::string &msg) {
    Log("Connection " + Utils::NbrToString(socket) + " closed. " + msg);
    // Explicitly delete socket from epoll instance to stop monitoring for events
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket, NULL);
    // Before closing the socket, shutdown the write side to send a FIN packet
    shutdown(socket, SHUT_RDWR);
    close(socket);
    connections_[socket] = Connection(is_running_, active_cgi_processes_);
}

void ServerManager::Log(const std::string &msg) const {
    std::cout << "[ " << Utils::Get().TimeElapsed() << " ] ";
    std::cout << msg << std::endl;
}

void ServerManager::PrintEventInfo(int events, int fd, int i) {
    epoll_events_count_++;
    if (events == 5)
        epoll_in_out_count_++;
    if (events == EPOLLIN && IsListeningSocketFd(fd))
        epoll_connection_count_++;

    std::cout << "\n== events " << epoll_events_count_ <<
    " == connections " << epoll_connection_count_ <<
    " == requests " << requests_made_ << "\n";

    if (cgifd_to_cl_sock_.find(fd) != cgifd_to_cl_sock_.end()) {
        int sock = connections_[cgifd_to_cl_sock_[fd]].connection_socket_;
        if (events == EPOLLOUT) {
            std::cout << "Client (" << sock << ") >>> " <<
                      " CGI (" << fd << ")" << std::endl;
        } else if (events == EPOLLIN) {
            std::cout << "Client (" << sock << ") <<< " <<
                      "CGI (" << fd << ")" << std::endl;
        }
    } else {
        std::cout << "Client (" << fd << ")" << std::endl;
    }

    if (cgifd_to_cl_sock_.size() != 0)
        std::cout << "cgi map size: " << cgifd_to_cl_sock_.size() << std::endl;
    if (closed_cgi_processes_ != 0)
        std::cout << "closed cgi processes: " << closed_cgi_processes_ << std::endl;
    if (opened_cgi_processes_ != 0)
        std::cout << "opened cgi processes: " << opened_cgi_processes_ << std::endl;
    if (active_cgi_processes_ != 0)
        std::cout << "tracked cgi processes: " << active_cgi_processes_ <<std::endl;

    std::cout << "nfd: " << i << "\n" << "fd: " << fd << "\n";
    if (events & EPOLLIN)
        std::cout << "EPOLLIN " << EPOLLIN << "\n";
    if (events & EPOLLPRI)
        std::cout << "EPOLLPRI " << EPOLLPRI << "\n";
    if (events & EPOLLOUT)
        std::cout << "EPOLLOUT " << EPOLLOUT << "\n";
    if (events & EPOLLRDNORM)
        std::cout << "EPOLLRDNORM " << EPOLLRDNORM << "\n";
    if (events & EPOLLRDBAND)
        std::cout << "EPOLLRDBAND " << EPOLLRDBAND << "\n";
    if (events & EPOLLWRNORM)
        std::cout << "EPOLLWRNORM " << EPOLLWRNORM << "\n";
    if (events & EPOLLWRBAND)
        std::cout << "EPOLLWRBAND " << EPOLLWRBAND << "\n";
    if (events & EPOLLMSG)
        std::cout << "EPOLLMSG " << EPOLLMSG << "\n";
    if (events & EPOLLERR)
        std::cout << "EPOLLERR " << EPOLLERR << "\n";
    if (events & EPOLLHUP)
        std::cout << "EPOLLHUP " << EPOLLHUP << "\n";
    if (events & EPOLLRDHUP)
        std::cout << "EPOLLRDHUP " << EPOLLRDHUP << "\n";
    if (events & EPOLLEXCLUSIVE)
        std::cout << "EPOLLEXCLUSIVE " << EPOLLEXCLUSIVE << "\n";
    if (events & EPOLLWAKEUP)
        std::cout << "EPOLLWAKEUP " << EPOLLWAKEUP << "\n";
    if (events & EPOLLONESHOT)
        std::cout << "EPOLLONESHOT " << EPOLLONESHOT << "\n";
}

bool    ServerManager::SetDescriptorNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        return false;
    }
    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

void ServerManager::Cleanup() {
    for (m_host_int::iterator it = host_to_socket_.begin();
            it != host_to_socket_.end(); ++it) {
        // Remove each listening socket from epoll_fd instance
        epoll_ctl(it->second, EPOLL_CTL_DEL, epoll_fd_, NULL);
        // close socket, no need to shutdown() here
        close(it->second);
    }
    close(epoll_fd_);
}

/**
 * Add new socket created by accept to epoll instance
 */
bool ServerManager::AddClientToEpoll(int client_sock) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = client_sock;
    if (SetDescriptorNonBlocking(client_sock))
        return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_sock, &event) != -1;
    Log("Can't set descriptor " + Utils::NbrToString(client_sock) +
        " to nonblocking mode.");
    return false;
}

bool ServerManager::AddCgiToEpoll(int cgi_fd, Connection &connection) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = cgi_fd;
    if (SetDescriptorNonBlocking(cgi_fd)) {
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, cgi_fd, &event) != -1) {
            // map connection with cgi_fd, so when cgi event will be
            // reported, we could find a corresponding connection
            cgifd_to_cl_sock_[cgi_fd] = connection.connection_socket_;
            return true;
        }
    }
    Log("Can't set descriptor " + Utils::NbrToString(cgi_fd) +
        " to nonblocking mode.");
    return false;
}

bool ServerManager::IsRealError(int fd) {
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    return err != 0;
}

