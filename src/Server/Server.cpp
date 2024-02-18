/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Server.cpp                                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:02:49 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
/**
        Description

        The setsockopt function can be used to set various socket options, each
    with its own specific data type. Here are some commonly used socket
    options and their corresponding data types:
        - SO_REUSEADDR: Controls the reuse of local addresses. It is typically
        used with TCP sockets and has an int data type. The opt variable in
        your code is of type int, and its value is set to 1 to enable the
        option.
        - SO_KEEPALIVE: Enables sending periodic keep-alive messages on the
        socket. It is typically used with TCP sockets and has an int data
        type. The opt variable can be set to 1 to enable the option or 0 to
        disable it.
        - SO_RCVBUF and SO_SNDBUF: Set the receive and send buffer sizes,
        respectively, for the socket. These options require an int data type.
        The opt variable can be set to the desired buffer size in bytes.
        - SO_LINGER: Specifies the behavior when closing the socket. It is used
        to control whether the socket lingers on closing, waiting for unsent
        data to be transmitted. It has a struct linger data type. You would
        need to create a linger structure and set its fields (l_onoff and
        l_linger) accordingly.
        - SO_BROADCAST: Enables sending broadcast messages on the socket. It
        is typically used with UDP sockets and has an int data type. The opt
        variable can be set to 1 to enable the option or 0 to disable it.
        - TCP_NODELAY: Disables Nagle's algorithm, which combines small
        outgoing TCP packets into a larger packet for efficiency. It is used
        with TCP sockets and has an int data type. The opt variable can be
        set to 1 to disable Nagle's algorithm or 0 to enable it.

        epoll is a scalable I/O event notification mechanism in Linux that allows
    monitoring multiple file descriptors for events efficiently. It is
    primarily used for building high-performance servers that handle a large
    number of concurrent connections.
    The epoll system call provided by the Linux kernel allows applications to
    register file descriptors (sockets, pipes, etc.) with an epoll instance
    and specify the events they are interested in (such as read, write, or
    error events). The epoll instance keeps track of these file descriptors
    and notifies the application when the specified events occur.
    One of the key advantages of epoll is its ability to efficiently handle a
    large number of file descriptors with a scalable event-driven model. It
    uses a data structure called an "event poll" that efficiently stores and
    manages the registered file descriptors. This allows for high-performance
    event notification and avoids the inefficiencies associated with
    traditional polling mechanisms like select or poll.

    The epoll interface provides three main functions:
        epoll_create: Creates an epoll instance and returns a file descriptor
    associated with it.
        epoll_ctl: Modifies or adds file descriptors to the epoll instance and
    specifies the events to monitor.
        epoll_wait: Waits for events to occur on the registered file descriptors
    and returns the events that have occurred.

    By using epoll, servers can efficiently handle a large number of
    connections and effectively manage I/O events, resulting in
    high-performance and scalable network applications.

    HTTP Methods:
   +-----------------------------------------------------------+
   | Method  | Description                                     |
   +---------+-------------------------------------------------+
   | GET     | Transfer a current representation of the target |
   |         | resource.                                       |
   | HEAD    | Same as GET, but only transfer the status line  |
   |         | and header section.                             |
   | POST    | Perform resource-specific processing on the     |
   |         | request payload.                                |
   | PUT     | Replace all current representations of the      |
   |         | target resource with the request payload.       |
   | DELETE  | Remove all current representations of the       |
   |         | target resource.                                |
   | CONNECT | Establish a tunnel to the server identified by  |
   |         | the target resource.                            |
   | OPTIONS | Describe the communication options for the      |
   |         | target resource.                                |
   | TRACE   | Perform a message loop-back test along the path |
   |         | to the target resource.                         |
   +---------+-------------------------------------------------+

 */
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <sys/fcntl.h>
#include "request/RequestExceptions.h"
#include "Server.h"

Server::Server(const Server &other)
: config_(other.config_), socket_(other.socket_), epoll_fd_(other.epoll_fd_),
  event_(other.event_) {}

 Server::Server(const ServerConfiguration &config)
: config_(config), socket_(0), epoll_fd_(0) {}

/**
 *  TODO MAX_CLIENTS
 */
void Server::Start() {
    Init();
    Log("Server initialized successfully..\n");
    std::cout << *this << std::endl;
    Start(config_.GetPort());
}

void Server::Start(int port) {
    if (epoll_fd_ > 0) {
        Log("started server at " + Utils::NbrToString(port) + " port");
        while (true) {HandleEvents();}
    } else {
        Log("It seems like " + Utils::NbrToString(port) +
            " port is already in use. Aborting.");
    }
    close(socket_);
}

bool    add_client_to_epoll(int client_sock, int epoll_fd) {
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_sock;
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) != -1;
}

bool    set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

void    Server::HandleEvents() {
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
    if (nfds == -1) {
        // Handle epoll_wait error
        return;
    }
    for (int i = 0; i < nfds; ++i) {
        if (events[i].data.fd == socket_) {
            // New connection
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(socket_,
                                     (struct sockaddr *) &client_addr,&client_len);
            CheckRequest(client_sock, client_addr);
        } else if (events[i].events & EPOLLIN) {
            HandleRequest(events[i].data.fd);
        }
    }
}

int Server::CheckRequest(int client_sock, const sockaddr_in &client_addr) {
    if (client_sock < 0) {
        Log("Error accepting connection!");
    } else if (//set_non_blocking(client_sock) &&
               add_client_to_epoll(client_sock, epoll_fd_)) {
        Log("Accepted client connection from " +
            Utils::NbrToString(client_addr.sin_addr.s_addr) + "\n");
    } else {
        Log("Error adding client socket to epoll of set nonblocking");
        close(client_sock);
    }
    return client_sock;
}

void Server::HandleRequest(int client_sock) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(config_.GetServerName(),
                             config_.GetPort());
    try {
        request.Init(client_sock);
        Log("Got client request:\n");
        std::cout << request << std::endl;
        response_location = ProcessRequest(request, client_sock);
        Log("Request processed");
    } catch (const HTTPVersionNotSupportedException &) {
        response_location.SetReturnCode(BAD_HTTP_VERSION);
    } catch (const ReadFromSocketFailedException &) {
        response_location.SetReturnCode(FAILED_IO);
    } catch (const ClientRequest::RequestException &) {
        response_location.SetReturnCode(BAD_REQUEST);
    }

    response.ComposeResponse(response_location);
    Log("Prepared response:\n");
    std::cout << response << std::endl;
    response.SendResponse(client_sock);
    Log("Response sent\n");
    close(client_sock);
}

void Server::Log(const std::string &msg) const {
    std::cout << "Server: " << msg << std::endl;
}

Server &Server::operator=(const Server &other) {
    if (this == &other)
        return *this;
    return *this;
}

Server::~Server() {}


const ServerConfiguration & Server::GetConfig() const {
    return config_;
}

int Server::GetSocket() const {
    return socket_;
}

int Server::GetEpollFd() const {
    return epoll_fd_;
}

std::ostream &operator<<(std::ostream &os, const Server &server) {
os << server.GetConfig() << "\n" <<
        "socket_: " << server.GetSocket() << "\n" <<
        "epoll_fd_: " << server.GetEpollFd();
    return os;
}