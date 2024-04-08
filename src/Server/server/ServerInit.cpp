/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerInit.cpp                                     :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 14:43:41 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib>

#include "../ServerManager.h"

/**
 * Create listening sockets for hosts present in current ServerConfiguration
 * @return 
 */
void ServerManager::CreateListeningSockets(int epoll_fd,
                                           const ServerConfiguration &conf) {
    for (s_hosts::const_iterator it = conf.GetHosts().begin();
            it != conf.GetHosts().end(); ++it) {
         if (host_to_socket_.find(*it) == host_to_socket_.end() ) {
            // socket for this host:port isn't already open
            struct addrinfo *addr = NULL;
            const std::string &port_str = Utils::NbrToString(it->port_);
            PresetAddress(&addr, it->host_, port_str, epoll_fd);
            int socket = CreateSocket(addr, it->host_, port_str, epoll_fd);
            SetSocketOptions(addr, socket, epoll_fd);
            BindSocket(addr, socket, epoll_fd);
            freeaddrinfo(addr);
            ListenSocket(socket, epoll_fd);
            AddSocketToEpollInstance(socket, epoll_fd);
        }
    }
}

/**
 * struct addrinfo {
 *     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
 *     int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
 *     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
 *     int              ai_protocol;  // use 0 for "any"
 *     size_t           ai_addrlen;   // size of ai_addr in bytes
 *     struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
 *     char            *ai_canonname; // full canonical hostname
 *
 *     struct addrinfo *ai_next;      // linked list, next node
 * };
 */
void ServerManager::PresetAddress(addrinfo **addr, const std::string &host,
                                  const std::string &port_str, int epoll_fd) {
    (void) epoll_fd;
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // Use the local IP
    hints.ai_family = AF_INET; // address family - Internet (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Stream socket (Not Datagram) TCP (not UDP)

    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, addr)) {
        std::string err_msg(strerror(errno));
//        Cleanup(epoll_fd);
        ThrowException("Failed getting address info for " + host + ":" +
                       port_str + " : " + err_msg);
    }
}

/**
 * Use the socket() function to create a new socket.
 * This function takes three arguments:
 *  the address family (PF_INET for IPv4),
 *  the socket type (e.g., SOCK_STREAM for a TCP socket),
 *  the protocol (usually set to 0 to let the operating system choose the
 *                                                        appropriate protocol).
 * The function returns a file descriptor that we can use to refer to the socket.
 */
int ServerManager::CreateSocket(addrinfo *res, const std::string &host,
                         const std::string &port_str, int epoll_fd) {
    (void) epoll_fd;
    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        std::string err_msg(strerror(errno));
//        Cleanup(epoll_fd);
        freeaddrinfo(res);
        ThrowException("Failed to create new socket for " + host + ":" +
                       port_str + " : " + err_msg);
    }
    host_to_socket_.insert(std::make_pair(
            Host(std::atoi(port_str.c_str()),host),
            sock));
    Log("Socket created");
    return sock;
}

/**
 *  Use the setsockopt() function to set socket options for the server socket.
 *  The level parameter (SOL_SOCKET) specifies the level at which the option is
 * defined, in this case the socket layer.
 *  The option parameter (SO_REUSEADDR) allows the socket to be bound to an
 * address that is already in use, as long as the original socket using that
 * @param res
 */
void ServerManager::SetSocketOptions(addrinfo *res, int socket, int epoll_fd) {
    (void) epoll_fd;
    int opt = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
//        Cleanup(epoll_fd);
//        ThrowException("Failed to set socket options for " +
//                       srv_sock_to_address_[socket] + " : " + err_msg, socket);
    }
    Log("Socket options set");
}

/**
 *  Use the bind() function to associate the socket with a specific port
 * number. This function takes two arguments: the socket file descriptor, and a
 * pointer to a sockaddr structure (res->ai_addr)
 * struct sockaddr {
 *     unsigned short    sa_family;    // address family, AF_xxx
 *     char              sa_data[14];  // 14 bytes of protocol address
 * };
 */
void ServerManager::BindSocket(addrinfo *res, int socket, int epoll_fd) {
    (void) epoll_fd;
    if (bind(socket, res->ai_addr, res->ai_addrlen)) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
//        Cleanup(epoll_fd);
//        ThrowException("Failed to Bind Socket for " +
//                       srv_sock_to_address_[socket] + " : " + err_msg, socket);
    }
    Log("Socket bind()-ed");
}

void ServerManager::ListenSocket(int socket, int epoll_fd) {
    (void) epoll_fd;
    if (listen(socket, SOMAXCONN) < 0) {
        std::string err_msg(strerror(errno));
//        Cleanup(epoll_fd);
//        ThrowException("Failed to Listen Socket for " +
//                       srv_sock_to_address_[socket] + " : " + err_msg, socket);
    }
    Log("Listening to socket fd:" + Utils::NbrToString(socket));
}

void ServerManager::AddSocketToEpollInstance(int socket, int epoll_fd) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event) < 0) {
        std::string err_msg(strerror(errno));
//        Cleanup(epoll_fd);
//        ThrowException("Failed to add socket on fd:" +
//                       Utils::NbrToString(socket) + " for " +
//                       srv_sock_to_address_[socket] + " to Epoll Instance : " +
//                       err_msg, socket);
    }
    Log("Listening socket added to epoll instance");
}
