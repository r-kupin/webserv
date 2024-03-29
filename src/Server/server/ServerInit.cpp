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

void Server::Init() {
    struct addrinfo *addr = NULL;

    CreateLogFile();
    PresetAddress(&addr);
    CreateSocket(addr);
    SetSocketOptions(addr);
    BindSocket(addr);
    freeaddrinfo(addr);
    ListenSocket();
    CreateEpoll();
    AddEpollInstance();
}

void Server::CreateLogFile() {
    std::string log_file_name = config_.GetServerName() + "_" +
                                Utils::NbrToString(config_.GetPort()) + ".log";
    if (!config_.GetLogDirAddress().empty())
        log_file_name = config_.GetLogDirAddress() + "/" + log_file_name;
    log_file_.open(log_file_name.c_str(), std::ios::out);
    if (!log_file_.is_open()) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to open log file " + config_.GetLogDirAddress() +
                       "/" + log_file_name + " : " + err_msg);
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
void Server::PresetAddress(addrinfo **addr) {
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // Use the local IP
    hints.ai_family = AF_INET; // address family - Internet (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Stream socket (Not Datagram) TCP (not UDP)

    if (getaddrinfo(config_.GetServerName().c_str(), // localhost
                    Utils::NbrToString(config_.GetPort()).c_str(), // port
                    &hints, addr)) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed getting address info : " + err_msg, log_file_);
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
void Server::CreateSocket(addrinfo *res) {
    socket_ = socket(res->ai_family,res->ai_socktype,0);
    if (socket_ < 0) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        ThrowException("Failed to create new socket : " + err_msg, log_file_);
    }
    Log("Socket created", log_file_);
}

/**
 *  Use the setsockopt() function to set socket options for the server socket.
 *  The level parameter (SOL_SOCKET) specifies the level at which the option is
 * defined, in this case the socket layer.
 *  The option parameter (SO_REUSEADDR) allows the socket to be bound to an
 * address that is already in use, as long as the original socket using that
 * @param res
 */
void Server::SetSocketOptions(addrinfo *res) {
    int opt = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        close(socket_);
        ThrowException("Failed to set socket options : " + err_msg, log_file_);
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
void Server::BindSocket(addrinfo *res) {
    if (bind(socket_, res->ai_addr, res->ai_addrlen)) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        close(socket_);
        ThrowException("Failed to Bind Socket : " + err_msg, log_file_);
    }
    Log("Socket bind()-ed", log_file_);
}

void Server::ListenSocket()  {
    if (listen(socket_, SOMAXCONN) < 0) {
        std::string err_msg(strerror(errno));
        close(socket_);
        ThrowException("Failed to Listen Socket : " + err_msg, log_file_);
    }
    Log("Listening to socket " + Utils::NbrToString(socket_), log_file_);
}

void Server::CreateEpoll() {
    epoll_fd_= epoll_create(1);
    if (epoll_fd_ < 0) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to Create Epoll Instance : " + err_msg,
                       log_file_);
    }
    Log("Epoll instance created", log_file_);
}

void Server::AddEpollInstance() {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = socket_;
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_, &event) < 0) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to add socket to Epoll Instance : " + err_msg,
                       log_file_);
    }
    Log("Listening socket added to epoll instance", log_file_);
}
