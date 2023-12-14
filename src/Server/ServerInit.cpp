#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include "ServerExceptions.h"
#include "Server.h"


void Server::Init() {
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
 * of `struct addrinfo` structures that contain the socket addresses that can
 * be used to establish a network connection.
 */
void Server::PresetAddress(addrinfo **addr) {
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // address family - Internet (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Stream socket (Not Datagram) TCP (not UDP)
    hints.ai_flags = AI_PASSIVE; // Use the local IP

    if (getaddrinfo(config_.server_name_.c_str(), // localhost
                    config_.port_str_.c_str(), // 8080
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
