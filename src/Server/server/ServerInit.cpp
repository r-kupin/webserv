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

#include "Server.h"

/**
 * Create listening sockets for all ports on all hosts
 * @return 
 */
void Server::Init(int epoll_fd) {
    for (s_str_c_it hn_it = config_.GetServerNames().begin();
         hn_it != config_.GetServerNames().end(); ++hn_it) {
        for (s_int_c_it p_it = config_.GetPorts().begin();
            p_it != config_.GetPorts().end(); ++p_it) {
            struct addrinfo *addr = NULL;
            const std::string &port_str = Utils::NbrToString(*p_it);

            PresetAddress(&addr, *hn_it, port_str);
            int socket = CreateSocket(addr, *hn_it, port_str);
            SetSocketOptions(addr, socket);
            BindSocket(addr, socket);
            freeaddrinfo(addr);
            ListenSocket(socket);
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
void Server::PresetAddress(addrinfo **addr, const std::string &host,
                           const std::string &port_str) {
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // Use the local IP
    hints.ai_family = AF_INET; // address family - Internet (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Stream socket (Not Datagram) TCP (not UDP)

    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, addr)) {
        std::string err_msg(strerror(errno));
        Cleanup(0);
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
int Server::CreateSocket(addrinfo *res, const std::string &host,
                          const std::string &port_str) {

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        std::string err_msg(strerror(errno));
        Cleanup(0);
        freeaddrinfo(res);
        ThrowException("Failed to create new socket for " + host + ":" +
                       port_str + " : " + err_msg);
    }
    srv_sock_to_address_.insert(std::make_pair(sock, host + ":" + port_str));
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
void Server::SetSocketOptions(addrinfo *res, int socket) {
    int opt = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        Cleanup(0);
        ThrowException("Failed to set socket options for " +
                       srv_sock_to_address_[socket] + " : " + err_msg);
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
void Server::BindSocket(addrinfo *res, int socket) {
    if (bind(socket, res->ai_addr, res->ai_addrlen)) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        Cleanup(0);
        ThrowException("Failed to Bind Socket for " +
                       srv_sock_to_address_[socket] + " : " + err_msg);
    }
    Log("Socket bind()-ed");
}

void Server::ListenSocket(int socket) {
    if (listen(socket, SOMAXCONN) < 0) {
        std::string err_msg(strerror(errno));
        Cleanup(0);
        ThrowException("Failed to Listen Socket for " +
                       srv_sock_to_address_[socket] + " : " + err_msg);
    }
    Log("Listening to socket fd:" + Utils::NbrToString(socket) +
        " for " + srv_sock_to_address_[socket] + " : ");
}

void Server::AddSocketToEpollInstance(int socket, int epoll_fd) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event) < 0) {
        std::string err_msg(strerror(errno));
        Cleanup(0);
        ThrowException("Failed to add socket on fd:" +
                       Utils::NbrToString(socket) + " for " +
                       srv_sock_to_address_[socket] + " to Epoll Instance : " +
                       err_msg);
    }
    Log("Listening socket added to epoll instance");
}
