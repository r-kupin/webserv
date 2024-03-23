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
/**
    HTTP Methods:
    +-----------------------------------------------------------+
    | Method  | Description                                     |
    +---------+-------------------------------------------------+
    | GET     | Transfer a current representation of the target |
    |         | resource.                                       |
    | POST    | Perform resource-specific processing on the     |
    |         | request payload.                                |
    | DELETE  | Remove all current representations of the       |
    |         | target resource.                                |
    |---------+-------------------------------------------------|
    | HEAD    | Same as GET, but only transfer the status line  |
    |         | and header section.                             |
    | PUT     | Replace all current representations of the      |
    |         | target resource with the request payload.       |
    | CONNECT | Establish a tunnel to the server identified by  |
    |         | the target resource.                            |
    | OPTIONS | Describe the communication options for the      |
    |         | target resource.                                |
    | TRACE   | Perform a message loop-back test along the path |
    |         | to the target resource.                         |
    +---------+-------------------------------------------------+
*/
#ifndef WEBSERV_SERVER_H
#define WEBSERV_SERVER_H

#include <exception>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <ostream>

#include "../../Config/config/Config.h"
#include "response/ServerResponse.h"
#include "../connection/Connection.h"

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

#define MAX_CLIENTS 2048
#define MAX_EVENTS 1000

class Server {
public:
    class ServerException : public std::exception {};

    Server(const Server &);
    explicit Server(const ServerConfiguration &config,
                    const volatile bool &is_running_ref);

    void                        Start();

    friend std::ostream         &operator<<(std::ostream &os, const Server &server);
protected:
//-------------------initialisation: open sockets, create epoll...--------------
    void                        Init();

    void                        CreateLogFile();
    void                        PresetAddress(addrinfo **addr);
    void                        CreateSocket(addrinfo *res);
    void                        SetSocketOptions(addrinfo *res);
    void                        BindSocket(addrinfo *res);
    void                        ListenSocket();
    void                        CreateEpoll();
    void                        AddEpollInstance();
//-------------------event handling---------------------------------------------
    void                        EventLoop();
    int                         CheckRequest(int client_sock);
    bool                        AddClientToEpoll(int client_sock);
    void                        HandleEvents(int client_sock);
//-------------------request server-side processing-----------------------------
    bool                        ProcessHeaders(int client_sock,
                                               Connection &connection);
    bool                        ProcessBody(int client_sock,
                                            Connection &connection);
    void                        Respond(int client_sock,
                                        const Connection &connection);
    Location                    ProcessRequest(ClientRequest &request, int socket);
    bool                        AccessForbidden(l_loc_c_it found,
                                                Methods method) const;
    bool                        RequestBodyExceedsLimit(l_loc_c_it found,
                                                        ClientRequest &request);
//-------------------static request processing----------------------------------
    void                        HandleStatic(const ClientRequest &request,
                                             const Srch_c_Res &res,
                                             const l_loc_c_it &found,
                                             Location &synth);
    void                        SynthIndex(Location &synth,
                                           const Srch_c_Res &res,
                                           int fs_status);
    std::string                 FindIndexToSend(const l_loc_c_it &found,
                                                const std::string &compliment) const;
    void                        SynthFile(Location &synth,
                                          const Srch_c_Res &res,
                                          int fs_status,
                                          const std::string &request_address);
//-------------------upload request processing----------------------------------
    int                         UploadFile(ClientRequest &request,
                                           l_loc_c_it found,
                                           int socket);
    int                         UploadFromCURL(ClientRequest &request,
                                               const std::string &filename,
                                               int socket);
    int                         PerformUpload(const ClientRequest &request,
                                              int socket, int file_fd,
                                              const std::string &delimiter);
    bool                        TryCreateOutputFile(const std::string &dir,
                                                    const std::string &filename,
                                                    size_t size);
    void                        HandleUpload(ClientRequest &request,
                                             int socket,
                                             l_loc_c_it &found,
                                             Location &synth);
//-------------------misc utils-------------------------------------------------
    bool                        SetDescriptorNonBlocking(int sockfd);
    void                        PrintEventInfo(int events, int fd, int i);
    void                        Log(const std::string & msg,
                                    std::ostream &os = std::cout) const;
    void                        CloseConnectionWithLogMessage(int client_sock,
                                                              const std::string &msg);
    void                        ThrowException(const std::string & msg,
                                               std::ostream &os = std::cout) const;
private:
    const volatile bool         &is_running_;
    const ServerConfiguration   &config_;
    int                         files_uploaded_;
    int                         socket_;
    int                         epoll_fd_;
    int                         epoll_returns_count_;
    int                         epoll_events_count_;
    int                         epoll_connection_count_;
    int                         epoll_in_out_count_;
    std::vector<Connection>     connections_;
    std::ofstream               log_file_;

    void NoUpoladDataAvailable(int file_fd, ssize_t bytes_read);
};

#endif //WEBSERV_SERVER_H
