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
#include <sstream>

#include "Server.h"

/**
 * Create listening sockets for all ports on all hosts
 * @return 
 */
bool Server::Init() {
    try {
        CreateLogFile();
        CreateEpollInstance();
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
                AddSocketToEpollInstance(socket);
            }
        }
    } catch (const ServerException &) {
        return false;
    }
    return true;
}

void Server::CreateLogFile() {
    std::stringstream ss;
    for (s_str_c_it it = config_.GetServerNames().begin();
        it != config_.GetServerNames().end(); ++it)
        ss << *it << "_";
    for (s_int_c_it it = config_.GetPorts().begin();
         it != config_.GetPorts().end(); ++it)
        ss << *it << "_";
    std::string log_file_name = ss.str() + ".log";
    if (!config_.GetLogDirAddress().empty())
        log_file_name = config_.GetLogDirAddress() + "/" + log_file_name;
    log_file_.open(log_file_name.c_str(), std::ios::out);
    if (!log_file_.is_open()) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to open log file " + config_.GetLogDirAddress() +
                       "/" + log_file_name + " : " + err_msg);
    }
}

void Server::CreateEpollInstance() {
    epoll_fd_= epoll_create(1);
    if (epoll_fd_ < 0) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to Create Epoll Instance : " + err_msg,
                       log_file_);
    }
    Log("Epoll instance created", log_file_);
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
        Cleanup();
        ThrowException("Failed getting address info for " + host + ":" +
                        port_str + " : " + err_msg, log_file_);
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
        Cleanup();
        freeaddrinfo(res);
        ThrowException("Failed to create new socket for " + host + ":" +
                       port_str + " : " + err_msg, log_file_);
    }
    socket_to_address_.insert(std::make_pair(sock, host + ":" + port_str));
    Log("Socket created", log_file_);
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
        Cleanup();
        ThrowException("Failed to set socket options for " +
                        socket_to_address_[socket] + " : " + err_msg, log_file_);
    }
    Log("Socket options set", log_file_);
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
        Cleanup();
        ThrowException("Failed to Bind Socket for " +
                    socket_to_address_[socket] + " : " + err_msg, log_file_);
    }
    Log("Socket bind()-ed", log_file_);
}

void Server::ListenSocket(int socket) {
    if (listen(socket, SOMAXCONN) < 0) {
        std::string err_msg(strerror(errno));
        Cleanup();
        ThrowException("Failed to Listen Socket for " +
                    socket_to_address_[socket] + " : " + err_msg, log_file_);
    }
    Log("Listening to socket fd:" + Utils::NbrToString(socket) +
        " for " + socket_to_address_[socket] + " : " , log_file_);
}

void Server::AddSocketToEpollInstance(int socket) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &event) < 0) {
        std::string err_msg(strerror(errno));
        Cleanup();
        ThrowException("Failed to add socket on fd:" +
                       Utils::NbrToString(socket) + " for " +
                       socket_to_address_[socket] + " to Epoll Instance : " +
                       err_msg, log_file_);
    }
    Log("Listening socket added to epoll instance", log_file_);
}
