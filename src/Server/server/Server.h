 /******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Server.h                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:02:49 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_SERVER_H
#define WEBSERV_LIB_SERVER_H

#include <exception>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include "../../Config/config/Config.h"
#include "../response/ServerResponse.h"

#define OK 200
#define REDIRECT 301
#define BAD_REQUEST 400
#define ACCESS_FORBIDDEN 403
#define NOT_FOUND 404
#define UNAPROPRIATE_METHOD 405
#define BODY_TOO_LARGE 413
#define REQUESTED_FILE_IS_NOT_A_FILE 500
#define FAILED_IO 500
#define ONLY_CURL_UPLOADS_SUPPORTED 501
#define FAILED_TO_CREATE_OUTPUT_FILE 503
#define BAD_HTTP_VERSION 505

#define MAX_CLIENTS 100
#define MAX_EVENTS 1000
#define SOCKET_BUFFER_SIZE 8192

 class Server {
public:
    class ServerException : public std::exception {};

    Server(const Server &);
    Server(const ServerConfiguration &config);

    Server &operator=(const Server &);

    ~Server();

    void                        Start();
    const ServerConfiguration   &GetConfig() const;
    int                         GetSocket() const;
    int                         GetEpollFd() const;

protected:
//-------------------initialisation: open sockets, create epoll...--------------
    void                        Init();
    void                        PresetAddress(addrinfo **addr);
    void                        CreateSocket(addrinfo *res);
    void                        SetSocketOptions(addrinfo *res) const;
    void                        BindSocket(addrinfo *res);
    void                        ListenSocket();
    void                        CreateEpoll();
    void                        AddEpollInstance();
//-------------------request handling-------------------------------------------
    void                        Start(int port);
    void                        HandleEvents();
    int                         CheckRequest(int client_sock,
                                             const sockaddr_in &client_addr);
    void                        HandleRequest(int client_sock);
//-------------------request server-side processing-----------------------------
    Location                    ProcessRequest(ClientRequest&request,
                                               int socket = -1);
    bool                        AccessForbidden(l_loc_c_it found,
                                                Methods method) const;
    bool                        RequestBodyExceedsLimit(l_loc_c_it found,
                                                        ClientRequest &request);
//-------------------static request processing----------------------------------
    void                        HandleStatic(const ClientRequest &request,
                                              const Srch_c_Res &res,
                                              const l_loc_c_it &found,
                                              Location &synth) const;
    void                        SynthIndex(Location &synth,
                                           const Srch_c_Res &res,
                                           int fs_status) const;
    std::string                 FindIndexToSend(const l_loc_c_it &found,
                                                const std::string &compliment) const;
    void                        SynthFile(Location &synth,
                                          const Srch_c_Res &res,
                                          int fs_status,
                                          const std::string &request_address) const;
//-------------------upload request processing----------------------------------
    int                         UploadFile(ClientRequest &request,
                                           l_loc_c_it found,
                                           int socket);
    int                         UploadFromCURL(ClientRequest &request,
                                               const std::string &filename,
                                               int socket);
    bool                        FlushBuffer(char *buffer, std::ofstream &file,
                                            const std::string &delimiter,
                                            int bytes_read);
    int                         FillBuffer(char *buffer, int socket,
                                           const size_t &size,
                                           v_char &storage) const;
    int                        PerformUpload(const ClientRequest &request,
                                             int socket, std::ofstream &file,
                                             const std::string &delimiter,
                                             char *buffer, size_t bytes_left);
    bool                        TryCreateOutputFile(const std::string &dir,
                                                    const std::string &filename,
                                                    size_t size) const;
    void                        HandleUpload(ClientRequest &request,
                                             int socket, l_loc_c_it &found,
                                             Location &synth);
private:
    void                        Log(const std::string & msg) const;

    const ServerConfiguration   &config_;
    int                         socket_;
    int                         epoll_fd_;
    epoll_event                 event_;
 };

std::ostream &operator<<(std::ostream &os, const Server &server);

#endif //WEBSERV_LIB_SERVER_H


//void Server::Start() {
//
//    while (true) {
//        struct sockaddr_in client_addr;
//        socklen_t client_len = sizeof(client_addr);
//        int client_sock = accept(socket_,
//                                 (struct sockaddr *) &client_addr,
//                                 &client_len); // may be [MAX_CLIENTS]
//        if (client_sock < 0) {
//            perror("accept");
//            printf("Error accepting connection: %s\n", strerror(errno));
//            sleep(10);
//            continue;
//        }
//
////    // Create array for incoming events
//    epoll_event events[MAX_EVENTS];
//
//    // Loop to handle incoming events
//    while (true) {
//        int numEvents = epoll_wait(epoll_fd_, events, MAX_EVENTS, 10);
//        if (numEvents == -1) {
//            std::cerr << strerror(errno) << std::endl;
//            std::cerr << "Error: Failed to wait for events\n";
//        } else if (numEvents == 0)
//            continue;
//
//        // Loop through all events
//        for (int i = 0; i < numEvents; ++i) {
//            int fd = events[i].data.fd;
//
//            // Check if event is an error
//            if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)) {
//                std::cerr << "Error: Event error on file descriptor " << fd << std::endl;
//                close(fd);
//                continue;
//            }
//
//            // Check if event is the server socket
//            if (fd == socket_) {
//                // Accept incoming connection(s)
//                while (true) {
//                    sockaddr_in clientAddr;
//                    socklen_t clientAddrSize = sizeof(clientAddr);
//                    int clientfd = accept(socket_, (sockaddr*)&clientAddr,
//                                          &clientAddrSize);
//                    if (clientfd == -1) {
//                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                            // No more incoming connections
//                            break;
//                        } else {
//                            std::cerr << "Error: Failed to accept incoming connection\n";
//                            break;
//                        }
//                    }
//                    // Add client socket to epoll instance
//                    event_.data.fd = clientfd;
//                    event_.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
//                    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, clientfd,
//                                  &event_) == -1) {
//                        std::cerr << "Error: Failed to add client socket to epoll instance\n";
//                        close(clientfd);
//                        continue;
//                    }
//                }
//            }  else {
//                // Read incoming data
//                char buffer[1024];
//                int bytesRead = 0;
//                while (true) {
//                    bytesRead = read(fd, buffer, 1024);
//                    if (bytesRead == -1) {
//                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                            // No more data to read
//                            break;
//                        }
//                        else {
//                            std::cerr << "Error: Failed to read from socket\n";
//                            break;
//                        }
//                    }
//                    else if (bytesRead == 0) {
//                        // Client has closed the connection
//                        std::cout << "Client on file descriptor " << fd << " has closed the connection\n";
//                        close(fd);
//                        break;
//                    }
//                    else {
//                        // Process incoming data
//                        std::cout << "Received " << bytesRead << " bytes of data from file descriptor " << fd << "\n";
//                        // ...
//                    }
//                }
//            }
//        }
//    }
//    }
//}



///**
// *  struct sockaddr_in is a data structure used to represent internet addresses.
// *
// *      The sin_family is an integer representing the address family, which
// *  in the case of sockaddr_in is always set to AF_INET for IPv4 addresses.
// *
// *      The sin_port member of struct sockaddr_in is used to specify the port
// *  number on which the socket should listen for incoming connections or
// *  the port number to which it should connect when initiating a connection.
// *      The htons() function is used to convert the port number from host byte
// *  order (the byte order used by the CPU of the computer running the
// *  program) to network byte order (the byte order used by the network).
// *
// *      The sin_addr field of struct sockaddr_in is used to specify the IP
// *  address of the socket. In this case, INADDR_ANY is used to specify that
// *  the socket can accept connections from any available network interface on
// *  the server.
// *      The htonl function is used to convert the INADDR_ANY constant from
// *  host byte order to network byte order. This is necessary because the
// *  sin_addr.s_addr field expects the IP address to be in network byte
// *  order, also known as big-endian byte order. By converting to network
// *  byte order, the value will be in the correct format for the
// *  sin_addr.s_addr field.
// *
// *      sockaddr_in is used with socket system calls, such as bind() and
// * connect(), to specify the address and port of the socket being bound or
// * connected to. It is also used in network programming to represent IP
// * addresses and port numbers in a format that can be easily converted
// * between binary and human-readable forms.
// *    Initialize a loop that runs indefinitely.
// *   Accept incoming client connections using the accept() function. This
// * blocks until a client connects to the server. If an error occurs during
// * the accept operation, the error is printed, and the loop waits for 10
// * seconds before continuing to the next iteration.
// *   Create an array to store incoming events using epoll_event
// * events[MAX_EVENTS].
// *   Use epoll_wait() to wait for events on the epoll instance. The
// * function blocks until events occur or a timeout of 10 milliseconds is
// * reached. The number of events that occurred is stored in numEvents.
// *   If an error occurs during epoll_wait(), an error message is printed.
// *   If no events occurred, continue to the next iteration of the loop.
// *   Iterate through the events that occurred.
// *   Retrieve the file descriptor associated with the event using int fd =
// * events[i].data.fd.
// *   Check if the event indicates an error (EPOLLERR or EPOLLHUP) or if
// * it's not a read event (EPOLLIN). If an error occurs, print an error
// * message, close the file descriptor, and continue to the next event.
// *   If the event is on the server socket (indicating a new connection),
// * accept the incoming connection(s) in a loop. If there are no more
// * incoming connections, break out of the loop.
// *   For each accepted client connection, add its socket to the epoll
// * instance using epoll_ctl() with the EPOLL_CTL_ADD operation. If an error
// * occurs, print an error message, close the client socket, and continue to
// * the next iteration of the loop.
// *   If the event is on a client socket, read incoming data from the socket
// * in a loop. If there is no more data to read, break out of the loop.
// *   If an error occurs during the read operation, handle the specific
// * error cases (EAGAIN or EWOULDBLOCK), print an error message, and break
// * out of the loop if necessary.
// *   If the read operation returns 0, it means the client has closed the
// * connection. Print a message indicating the closed connection, close the
// * client socket, and break out of the loop.
// *   If data is successfully read, process the incoming data (in this case,
// * it only prints the received data).
// *   Repeat the loop to wait for more events.
// * This code represents a basic
// *server implementation using epoll for event-driven I/O. It accepts client
// *connections, adds the client sockets to the epoll instance, and reads
// *incoming data from the client sockets. The implementation handles errors
// *and gracefully closes sockets when necessary.
// */
//void Server::Start() {
//
//    while (true) {
//        struct sockaddr_in client_addr;
//        socklen_t client_len = sizeof(client_addr);
//        int client_sock = accept(socket_,
//                                 (struct sockaddr *) &client_addr,
//                                 &client_len); // may be [MAX_CLIENTS]
//        if (client_sock < 0) {
//            perror("accept");
//            printf("Error accepting connection: %s\n", strerror(errno));
//            sleep(10);
//            continue;
//        }
//
//    // Create array for incoming events
//    epoll_event events[MAX_EVENTS];
//
//    // Loop to handle incoming events
//    while (true) {
//        int numEvents = epoll_wait(epoll_fd_, events, MAX_EVENTS, 10);
//        if (numEvents == -1) {
//            std::cerr << strerror(errno) << std::endl;
//            std::cerr << "Error: Failed to wait for events\n";
//        } else if (numEvents == 0)
//            continue;
//
//        // Loop through all events
//        for (int i = 0; i < numEvents; ++i) {
//            int fd = events[i].data.fd;
//
//            // Check if event is an error
//            if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)) {
//                std::cerr << "Error: Event error on file descriptor " << fd << std::endl;
//                close(fd);
//                continue;
//            }
//
//            // Check if event is the server socket
//            if (fd == socket_) {
//                // Accept incoming connection(s)
//                while (true) {
//                    sockaddr_in clientAddr;
//                    socklen_t clientAddrSize = sizeof(clientAddr);
//                    int clientfd = accept(socket_, (sockaddr*)&clientAddr,
//                                          &clientAddrSize);
//                    if (clientfd == -1) {
//                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                            // No more incoming connections
//                            break;
//                        } else {
//                            std::cerr << "Error: Failed to accept incoming connection\n";
//                            break;
//                        }
//                    }
//                    // Add client socket to epoll instance
//                    event_.data.fd = clientfd;
//                    event_.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
//                    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, clientfd,
//                                  &event_) == -1) {
//                        std::cerr << "Error: Failed to add client socket to epoll instance\n";
//                        close(clientfd);
//                        continue;
//                    }
//                }
//            }  else {
//                // Read incoming data
//                char buffer[1024];
//                int bytesRead = 0;
//                while (true) {
//                    bytesRead = read(fd, buffer, 1024);
//                    if (bytesRead == -1) {
//                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                            // No more data to read
//                            break;
//                        }
//                        else {
//                            std::cerr << "Error: Failed to read from socket\n";
//                            break;
//                        }
//                    }
//                    else if (bytesRead == 0) {
//                        // Client has closed the connection
//                        std::cout << "Client on file descriptor " << fd << " has closed the connection\n";
//                        close(fd);
//                        break;
//                    }
//                    else {
//                        // Process incoming data
//                        std::cout << "Received " << bytesRead << " bytes of data from file descriptor " << fd << "\n";
//                        // ...
//                    }
//                }
//            }
//        }
//    }
//    }
//}