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

     The setsockopt function can be used to set various socket options, each with its own specific data type. Here are some commonly used socket options and their corresponding data types:

    SO_REUSEADDR: Controls the reuse of local addresses. It is typically used with TCP sockets and has an int data type. The opt variable in your code is of type int, and its value is set to 1 to enable the option.

    SO_KEEPALIVE: Enables sending periodic keep-alive messages on the socket. It is typically used with TCP sockets and has an int data type. The opt variable can be set to 1 to enable the option or 0 to disable it.

    SO_RCVBUF and SO_SNDBUF: Set the receive and send buffer sizes, respectively, for the socket. These options require an int data type. The opt variable can be set to the desired buffer size in bytes.

    SO_LINGER: Specifies the behavior when closing the socket. It is used to control whether the socket lingers on closing, waiting for unsent data to be transmitted. It has a struct linger data type. You would need to create a linger structure and set its fields (l_onoff and l_linger) accordingly.

    SO_BROADCAST: Enables sending broadcast messages on the socket. It is typically used with UDP sockets and has an int data type. The opt variable can be set to 1 to enable the option or 0 to disable it.

    TCP_NODELAY: Disables Nagle's algorithm, which combines small outgoing TCP packets into a larger packet for efficiency. It is used with TCP sockets and has an int data type. The opt variable can be set to 1 to disable Nagle's algorithm or 0 to enable it.

    epoll is a scalable I/O event notification mechanism in Linux that allows monitoring multiple file descriptors for events efficiently. It is primarily used for building high-performance servers that handle a large number of concurrent connections.

The epoll system call provided by the Linux kernel allows applications to register file descriptors (sockets, pipes, etc.) with an epoll instance and specify the events they are interested in (such as read, write, or error events). The epoll instance keeps track of these file descriptors and notifies the application when the specified events occur.

One of the key advantages of epoll is its ability to efficiently handle a large number of file descriptors with a scalable event-driven model. It uses a data structure called an "event poll" that efficiently stores and manages the registered file descriptors. This allows for high-performance event notification and avoids the inefficiencies associated with traditional polling mechanisms like select or poll.

The epoll interface provides three main functions:

    epoll_create: Creates an epoll instance and returns a file descriptor associated with it.
    epoll_ctl: Modifies or adds file descriptors to the epoll instance and specifies the events to monitor.
    epoll_wait: Waits for events to occur on the registered file descriptors and returns the events that have occurred.

By using epoll, servers can efficiently handle a large number of connections and effectively manage I/O events, resulting in high-performance and scalable network applications.
 */
#include <iostream>
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include "ServerExceptions.h"
#include "ClientRequest.h"

//const int BUFFER_SIZE = 1024;
//const int MAX_EVENTS = 10;

Server::Server() {}

Server::Server(const Server &other)
: config_(other.config_), socket_(other.socket_), epoll_fd_(other.epoll_fd_),
  event_(other.event_) {}

 Server::Server(const ServerConfiguration &config,
                const std::map<int, std::string> *http_codes)
: kHttpCodes(http_codes), config_(config), socket_(0), epoll_fd_(0) {
    try {
        struct addrinfo *addr = NULL;

        PresetAddress(&addr);
        CreateSocket(addr);
        SetSocketOptions(addr);
        BindSocket(addr);

        freeaddrinfo(addr);

        ListenSocket();
        CreateEpoll();
        AddEpollInstance();
    } catch (const AddrinfoCreationFailed &) {
        std::cout << "Failed getting address info! Unable to resolve a domain"
                     " name or IP address or port" << std::endl;
    } catch (const SocketOpeningFailureException &) {
        std::cout << "Failed to create new socket" << std::endl;
    } catch (const SocketSetOptionsFailureException &) {
        std::cout << "Failed to set socket options" << std::endl;
    } catch (const SocketBindingFailureException &) {
        std::cout << "Failed to Bind Socket" << std::endl;
    } catch (const SocketListeningFailureException &) {
        std::cout << "Failed to Listen Socket" << std::endl;
    } catch (const EpollCreationFailed &) {
        std::cout << "Failed to Create Epoll Instance" << std::endl;
    } catch (const EpollAddFailed &) {
        std::cout << "Failed to add socket to Epoll Instance" << std::endl;
    }
}

/**
 *  getaddrinfo() is a function provided by the operating system's networking
 * stack that resolves a domain name or IP address to a list of socket
 * addresses that can be used to establish a network connection.
 *  The getaddrinfo() function takes as input a hostname or IP address, a
 * service name or port number, and a set of hints that specify the desired
 * address family, socket type, and protocol. It then returns a linked list
 * of struct addrinfo structures that contain the socket addresses that can
 * be used to establish a network connection.
 */
void Server::PresetAddress(addrinfo **addr) {
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Use the local IP

    if (getaddrinfo(config_.hostname_.c_str(),config_.port_str_.c_str(),
                    &hints, addr)) {
        throw AddrinfoCreationFailed();
    }

//    char ipstr[INET_ADDRSTRLEN];
//    struct sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>
//            ((*addr)->ai_addr);
//    inet_ntop(AF_INET, &(addr_in->sin_addr), ipstr, INET_ADDRSTRLEN);
//    std::cout << "Server address: " << ipstr << '\n';
//    std::cout << "Server port: " << ntohs(addr_in->sin_port) << '\n';
}

/**
 * Use the socket() function to create a new socket.
 * This function takes three arguments:
 *  the address family (AF_INET for IPv4),
 *  the socket type (e.g., SOCK_STREAM for a TCP socket),
 *  the protocol (usually set to 0 to let the operating system choose the
 *                                                        appropriate protocol).
 * The function returns a file descriptor that we can use to refer to the socket.
 */
void Server::CreateSocket(addrinfo *res) {
    socket_ = socket(res->ai_family,res->ai_socktype,0);
    if (socket_ < 0) {
        freeaddrinfo(res);
        throw SocketOpeningFailureException();
    }
}

/**
 *  Use the setsockopt() function to set socket options for the server socket.
 *  The level parameter (SOL_SOCKET) specifies the level at which the option is
 * defined, in this case the socket layer.
 *  The option parameter (SO_REUSEADDR) allows the socket to be bound to an
 * address that is already in use, as long as the original socket using that
 * @param res
 */
void Server::SetSocketOptions(addrinfo *res) const {
    int opt = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        freeaddrinfo(res);
        close(socket_);
        throw SocketSetOptionsFailureException();
    }
}

/**
 *  Use the bind() function to associate the socket with a specific port
 * number. This function takes two arguments: the socket file descriptor, and a
 * pointer to a sockaddr structure that contains the IP address and port number
 * to bind to.
 */
void Server::BindSocket(addrinfo *res) {
    if (bind(socket_, res->ai_addr, res->ai_addrlen)) {
        freeaddrinfo(res);
        close(socket_);
        throw SocketBindingFailureException();
    }
}

/**
 *  The listen() function is a system call in the Linux/Unix operating
 * systems that sets up the specified socket to listen for incoming client
 * connections.
 *  The function takes two arguments: the first is the file descriptor of
 * the socket, and the second is the maximum number of client connections that
 * can be queued up and waiting for the server to accept them.
 *  Once the socket has been created and bound to a specific IP address and
 * port number, the listen() function enables the socket to begin accepting
 * incoming client connections. When a client connects to the server, the
 * connection is placed in a queue until the server is able to accept the
 * connection. The size of the queue is determined by the second argument
 * passed to the listen() function.
 *  When a connection is accepted by the server, a new socket is created for
 * communication between the client and the server. The server can then use
 * this new socket to send and receive data with the client. The original socket
 * created by the socket() function remains open and continues to listen for
 * new incoming connections.
 */
void Server::ListenSocket()  {
    if (listen(socket_, 5) < 0) {
        close(socket_);
        throw SocketListeningFailureException();
    }
}

/**
 *  The epoll_create system call is used to create an epoll instance and
 * returns a file descriptor associated with it. Under the hood, epoll_create
 * interacts with the Linux kernel to set up the necessary data structures
 * and resources for the epoll instance.
 */
void Server::CreateEpoll() {
    epoll_fd_= epoll_create(1);
    if (epoll_fd_ < 0)
        throw EpollCreationFailed();
}

/**
 *  The epoll_ctl function interacts with the Linux kernel to manipulate the
 * epoll instance and the registered file descriptors. It allows to add,
 * modify, or remove file descriptors from the epoll instance and specify
 * the events to monitor for each file descriptor
 *  The events in event_.events represent the types of events that the epoll
 * instance should monitor for a specific file descriptor. These events
 * indicate the readiness or availability of certain operations on the file
 * descriptor. Let's break down each event flag you mentioned:
 *  EPOLLIN: This event flag indicates that there is data available to be
 * read from the file descriptor without blocking. It typically means that a
 * read operation can be performed on the file descriptor.
 *  EPOLLET: This event flag enables edge-triggered behavior for the
 * associated file descriptor. With edge-triggered mode, an event is
 * triggered only when the state of the file descriptor changes, such as when
 * new data arrives or when a connection is established. It requires actively
 * reading or writing data from the file descriptor until it returns an error
 * (e.g., EAGAIN or EWOULDBLOCK) to avoid missing events.
 *  EPOLLONESHOT: This event flag ensures that the file descriptor is
 * monitored for events only once until it is re-armed. After an event is
 * triggered for the file descriptor, it is automatically disabled in the
 * epoll instance. To monitor for events again, the file descriptor needs to
 * be re-registered using EPOLL_CTL_MOD with the desired events. This flag is
 * useful in scenarios where you want to handle each event only once.
 */
void Server::AddEpollInstance() {
    std::memset(&event_, 0, sizeof(event_));
    event_.data.fd = socket_;
    event_.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD,
                  socket_, &event_) < 0)
        throw EpollAddFailed();
}

/**
 *  TODO MAX_CLIENTS
 */
 void Server::Start() {
     std::cout << "started server at " << config_.port_ << " port" << std::endl;
     while (true) {
         struct sockaddr_in client_addr;
         socklen_t client_len = sizeof(client_addr);
         int client_sock = accept(socket_,
                                  (struct sockaddr *) &client_addr,
                                  &client_len); // may be [MAX_CLIENTS]
         if (client_sock < 0) {
             printf("Error accepting connection: %s\n", strerror(errno));
             continue;
         }
         std::cout << "Accepted client connection from " <<
                                    client_addr.sin_addr.s_addr << std::endl;
         try {
             HandleClientRequest(client_sock);
         } catch (ReadFromSocketFailedException) {
             std::cout << "Read from socket failed!" << std::endl;
         }
         close(client_sock);
     }
     close(socket_);
 }

const Location &Server::FindLocation(const std::string &uri,
                                     const Location &start,
                                     int &http_code) {
     if (uri != start.address_) {
         std::string part_uri = uri.substr(1);
         std::string::size_type end = part_uri.find('/');
         if (end == std::string::npos)
             end = uri.size();

         part_uri = ("/" + part_uri).substr(0, end + 1);
         const std::set<Location>::iterator &it =
                     start.sublocations_.find(Location(part_uri));
         if (it != start.sublocations_.end()) {
             return FindLocation(uri.substr(end + 1), *it, http_code);
         } else {
             http_code = 404;
             return start;
         }
     }
     http_code = 200;
     return start;
}

void Server::HandleClientRequest(int client_sock) {
     try {
         ClientRequest request(client_sock);
         int http_code;
         const Location &loc = FindLocation(request.uri_, config_.root_loc_,
                                            http_code);
         std::cout << loc.address_ << " " <<http_code << std::endl;
//////         const Location &loc = (request.uri_ == "/") ? config_.root_loc_ :
//////         *(config_.root_loc_.sublocations_.find(Location(request.uri_)));
////
////         if (it == config_.locations_.end()) {
//////             return 404
////         } else {
////             const Location &to_go = *(it);
////             if (!to_go.limit_except_.except_.empty() &&
////                    to_go.limit_except_.except_.find(request.method_) ==
////                            to_go.limit_except_.except_.end()) {
//////                 return to_go.return_code_;
////             }
//         }
//         std::string filepath;
//         if ()
//         = kDefaultResPath +
     } catch (const ReadFromSocketFailedException &) {
         std::cout << "Read from client socket failed!" << std::endl;
     } catch (const UnsupportedClientMethodException &) {
         std::cout << "Read from client socket failed!" << std::endl;
     } catch (const HTTPVersionNotSupportedException &) {
         std::cout << "Read from client socket failed!" << std::endl;
     } catch (const NotFoundException &) {
//         return 404
         std::cout << "Not Found!" << std::endl;
     }

//     // Open file corresponding to requested URI
//     std::string filepath = "resources/default/htmls";
//     std::ifstream file(filepath.c_str());
//     if (!file.good()) {
//         // If file doesn't exist, send 404 error response to client
//         std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
//         send(client_sock, response.c_str(), response.length(), 0);
//         return;
//     }
//
//     std::string response = "HTTP/1.1 200 OK\r\n\r\n";
////     while (!file.eof()) {
////         file.read(buffer, BUFFER_SIZE);
////         send(client_sock, buffer, file.gcount(), 0);
////     }
//
//     // Close file and socket
//     file.close();
//     close(client_sock);
 }

Server &Server::operator=(const Server &other) {
    if (this == &other)
        return *this;
    return *this;
}

Server::~Server() {}

const char *SocketOpeningFailureException::what() const throw() {
    return exception::what();
}

const char *SocketBindingFailureException::what() const throw() {
    return exception::what();
}

const char *SocketListeningFailureException::what() const throw() {
    return exception::what();
}

const char *AddrinfoCreationFailed::what() const throw() {
    return exception::what();
}

const char *SocketSetOptionsFailureException::what() const throw() {
    return exception::what();
}

const char *EpollCreationFailed::what() const throw() {
    return exception::what();
}

const char *EpollAddFailed::what() const throw() {
    return exception::what();
}

const char *ReadFromSocketFailedException::what() const throw() {
    return exception::what();
}

const char *UnsupportedClientMethodException::what() const throw() {
    return exception::what();
}

const char *HTTPVersionNotSupportedException::what() const throw() {
    return exception::what();
}

const char *NotFoundException::what() const throw() {
    return exception::what();
}
