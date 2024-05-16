/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManagerInit.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/31 15:57:14 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "ServerManager.h"

// Initializes the ServerManager with the provided configuration.
// This includes setting up signal handlers for SIGINT and SIGSTOP to gracefully stop the servers,
// and creating server instances based on the configuration provided.
void ServerManager::Init(const Config &config) {
    // Setup signal handlers to allow for graceful shutdown on SIGINT (Ctrl+C) and SIGSTOP signals.
    signal(SIGINT, Stop);
    signal(SIGSTOP, Stop);
    signal(SIGPIPE, Stop);

    // Current version uses only 1 epoll instance to monitor all connections
    // on all servers
    CreateEpollInstance();
    // Iterate over the server configurations provided in 'config' to create server instances.
    // These instances are stored in the 'servers_' vector for management.
    for (l_sc_c_it it = config.getConstServers().begin();
         it != config.getConstServers().end(); ++it) {
        try {
            // Open sockets for the current configuration and map them to
            // hosts if they aren't open yet
            CreateListeningSocket(*it);
            // Create Server with particular config
            servers_.push_back(Server(*it, is_running_, host_to_socket_,
                                      *this));
            std::cout << servers_.back() << std::endl;
        } catch (const Server::ServerException &) {
            Cleanup();
            ThrowException("Server initialisation failed");
        }
    }
    // Create Connection instances for each fd dedicated to keep the state of
    // communication between the client and server
    for (int i = 0; i < CONNECTIONS; ++i) {
        connections_.push_back(Connection(is_running_, active_cgi_processes_));
    }
    std::cout << "Servers created successfully!" << std::endl;
}

void ServerManager::CreateEpollInstance() {
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ < 0) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to Create Epoll Instance : " + err_msg);
    }
    Log("Epoll instance created");
}

/**
 * Create listening sockets for hosts present in current ServerConfiguration
 * current version allows to avoid opening of the multiple sockets for the
 * same host on different servers
 */
void ServerManager::CreateListeningSocket(const ServerConfiguration &conf) {
    for (s_hosts::const_iterator it = conf.GetHosts().begin();
         it != conf.GetHosts().end(); ++it) {
        if (host_to_socket_.find(*it) == host_to_socket_.end() ) {
            // socket for this host:port isn't already open
            struct addrinfo *addr = NULL;
            const std::string &port_str = Utils::NbrToString(it->port_);
            PresetAddress(&addr, it->host_, port_str);
            int socket = CreateSocket(addr, it->host_, port_str);
            SetSocketOptions(addr, socket);
            BindSocket(addr, socket);
            freeaddrinfo(addr);
            ListenSocket(socket);
            AddSocketToEpollInstance(socket);
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
                                  const std::string &port_str) {
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // Use the local IP
    hints.ai_family = AF_INET; // address family - Internet (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Stream socket (Not Datagram) TCP (not UDP)

    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, addr)) {
        std::string err_msg(strerror(errno));
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
                                const std::string &port_str) {
    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        std::string err_msg(strerror(errno));
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
void ServerManager::SetSocketOptions(addrinfo *res, int socket) {
    int opt = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        ThrowException("Failed to set socket options : " + err_msg);
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
void ServerManager::BindSocket(addrinfo *res, int socket) {
    if (bind(socket, res->ai_addr, res->ai_addrlen)) {
        std::string err_msg(strerror(errno));
        freeaddrinfo(res);
        ThrowException("Failed to bind socket for  : " + err_msg);
    }
    Log("Socket bind()-ed");
}

void ServerManager::ListenSocket(int socket) {
    if (listen(socket, SOMAXCONN) < 0) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to Listen Socket : " + err_msg);
    }
    Log("Listening to socket fd:" + Utils::NbrToString(socket));
}

void ServerManager::AddSocketToEpollInstance(int socket) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &event) < 0) {
        std::string err_msg(strerror(errno));;
        ThrowException("Failed to add socket on fd:" +
                       Utils::NbrToString(socket) + " to Epoll Instance : " +
                       err_msg);
    }
    Log("Listening socket added to epoll instance");
}
