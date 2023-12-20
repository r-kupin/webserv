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
#include "ServerExceptions.h"
#include "request/RequestExceptions.h"

//const int BUFFER_SIZE = 1024;
//const int MAX_EVENTS = 10;
//
Server::Server()
: socket_(0), epoll_fd_(0) {}

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
    Start(config_.GetPort());
}

void Server::Start(int port) {
    std::cout << "started server at " << port << " port" << std::endl;
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(socket_,
                                 (struct sockaddr *) &client_addr,
                                 &client_len); // may be [MAX_CLIENTS]
        CheckRequest(client_sock, client_addr);
    }
    close(socket_);
}

void Server::CheckRequest(int client_sock, const sockaddr_in &client_addr) {
    if (client_sock < 0) {
        std::cout << "Error accepting connection! " << std::endl;
    } else {
        std::cout << "Accepted client connection from " <<
                                    client_addr.sin_addr.s_addr << std::endl;
        try {
            HandleClientRequest(client_sock);
        } catch (const ClientRequest::RequestException &) {
            std::cout << "Read from client socket failed!" << std::endl;
        } catch (const ServerResponse::ResponseException &) {
            std::cout << "Response creation failed!" << std::endl;
        }
        close(client_sock);
    }
}

void Server::HandleClientRequest(int client_sock) {
    ClientRequest request(client_sock);
    ServerResponse response(request, SynthesizeHandlingLocation(request));
//         response.SendResponse(client_sock);


//         if (it == config_.locations_.end()) {
////             return 404
//         } else {
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


void Server::SetConfig(const ServerConfiguration &config) {
    config_ = config;
}

const ServerConfiguration & Server::GetConfig() {
    return config_;
}

int Server::getSocket() const {
    return socket_;
}

int Server::getEpollFd() const {
    return epoll_fd_;
}

const epoll_event &Server::getEvent() const {
    return event_;
}


//       ClientRequest request(client_sock);
//         int http_code;
//         const Location &loc = FindSublocation(request.address_,
//                                            config_.locations_,
//                                            http_code);
//         ServerResponse response(request, config_.locations_, http_code);
//
//
//
//         html.exceptions(std::ifstream::failbit);
//         header <<  "HTTP/1.1 " << http_code << " ";
//         if (Location::kHttpOkCodes.find(http_code) !=
//                                                Location::kHttpOkCodes.end()) {
//             header << Location::kHttpOkCodes.find(http_code)->second;
//             if (!loc.index_.empty()) {
//                 for (std::set<std::string>::iterator it = loc.index_.begin();
//                      it != loc.index_.end(); ++it) {
//                     std::cout << kDefaultResPath + loc.root_ + *it << std::endl;
//                     html.open((kDefaultResPath + loc.root_ + *it).c_str());
//                     if (html) {
//                         html.exceptions(std::ifstream::badbit);
//                         break;
//                     }
//                 }
//             }
//         } else if (ErrPage::kHttpErrCodes.find(http_code) !=
//                                                ErrPage::kHttpErrCodes.end()) {
//             header << ErrPage::kHttpErrCodes.find(http_code)->second;
//             const std::set<ErrPage>::iterator &err_page =
//                            loc.error_pages_.find(ErrPage(http_code));
//             if (err_page != loc.error_pages_.end()) {
//                 html.open((kDefaultResPath + loc.root_ + err_page->address_).c_str());
//                 if (html) {
//                     html.exceptions(std::ifstream::badbit);
//                 }
//             }
//         }
//         header << " \r\n\r\n";
//

// // Create a buffer to hold the file data
//         const size_t bufferSize = 1024;
//         char buffer[bufferSize];
//
//         memccpy(buffer, header.str().c_str(), '\0', header.str().size());
//         send(client_sock, buffer, header.str().size(), 0);
//
//
//         // Read and send the file data in chunks
//         while (fileSize > 0) {
//             // Determine the number of bytes to read
//             size_t bytesRead = std::min(static_cast<size_t>(bufferSize),
//                                         static_cast<size_t>(fileSize));
//
//             // Read data from the file
//             html.read(buffer, bytesRead);
//
//             // Send the data over the socket
//             ssize_t bytesSent = send(client_sock, buffer, bytesRead, 0);
//             if (bytesSent < 0) {
//                 std::cerr << "Failed to send data over the socket"
//                           << std::endl;
//                 break;
//             }
//
//             // Update the remaining file size
//             fileSize -= bytesSent;
//         }

