/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    AServer.h                                          :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 14:43:41 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
/*
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
#ifndef WEBSERV_LIB_ASERVER_H
#define WEBSERV_LIB_ASERVER_H

#include <exception>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <ostream>
#include "../../Config/config/Config.h"
#include "../response/ServerResponse.h"
#include "AServer.h"

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

static volatile bool        is_running_ = true;

class AServer {
public:
    class ServerException : public std::exception {};

    AServer(const AServer &);
    explicit AServer(const ServerConfiguration &config);

    virtual ~AServer();

    void                        Start();
    static void                 Stop(int signal);
    const ServerConfiguration   &GetConfig() const;
    int                         GetSocket() const;
    int                         GetEpollFd() const;

    friend std::ostream &operator<<(std::ostream &os, const AServer &server);
protected:
//-------------------initialisation: open sockets, create epoll...--------------
    void                        Init();
    void                        PresetAddress(addrinfo **addr);
    void                        CreateSocket(addrinfo *res);
    void                        SetSocketOptions(addrinfo *res) const;
    void                        BindSocket(addrinfo *res);
    void                        ListenSocket();
    void                        CreateEpoll();
    virtual void                AddEpollInstance() = 0;
//-------------------request handling-------------------------------------------
    void                        Start(int port);
    int                         CheckRequest(int client_sock);
    virtual bool                AddClientToEpoll(int client_sock, int epoll_fd) = 0;
    void                        HandleEvents();
    virtual void                HandleRequest(int client_sock) = 0;
//-------------------request server-side processing-----------------------------
    Location                    ProcessRequest(ClientRequest &request,
                                               std::ostream &os,
                                               int socket = -1);
    bool                        AccessForbidden(l_loc_c_it found,
                                                Methods method) const;
    bool                        RequestBodyExceedsLimit(l_loc_c_it found,
                                                        ClientRequest &request);
//-------------------static request processing----------------------------------
    void                        HandleStatic(const ClientRequest &request,
                                             const Srch_c_Res &res,
                                             const l_loc_c_it &found,
                                             Location &synth,
                                             std::ostream &os) const;
    void                        SynthIndex(Location &synth,
                                           const Srch_c_Res &res,
                                           int fs_status,
                                           std::ostream &os) const;
    std::string                 FindIndexToSend(const l_loc_c_it &found,
                                                const std::string &compliment) const;
    void                        SynthFile(Location &synth,
                                          const Srch_c_Res &res,
                                          int fs_status,
                                          const std::string &request_address,
                                          std::ostream &os) const;
//-------------------upload request processing----------------------------------
    int                         UploadFile(ClientRequest &request,
                                           l_loc_c_it found,
                                           int socket,
                                           std::ostream &os);
    int                         UploadFromCURL(ClientRequest &request,
                                               const std::string &filename,
                                               int socket, std::ostream &os);
    bool                        FlushBuffer(char *buffer, int file_fd,
                                            const std::string &delimiter,
                                            int bytes_read);
    int                         FillBuffer(char *buffer, int socket,
                                           const size_t &size,
                                           v_char &storage) const;
    int                         PerformUpload(const ClientRequest &request,
                                              int socket,
                                              int file_fd,
                                              const std::string &delimiter,
                                              char *buffer,
                                              size_t bytes_left,
                                              std::ostream &os);
    bool                        TryCreateOutputFile(const std::string &dir,
                                                    const std::string &filename,
                                                    size_t size,
                                                    std::ostream &os) const;
    void                        HandleUpload(ClientRequest &request,
                                             int socket,
                                             l_loc_c_it &found,
                                             Location &synth,
                                             std::ostream &os);
    void                        Log(const std::string & msg,
                                    std::ostream &os = std::cout) const;
private:
    const ServerConfiguration   &config_;
    int                         socket_;
    int                         epoll_fd_;
    int                         epoll_returns_count_;
    int                         epoll_events_count_;
    int                         epoll_connection_count_;
    int                         epoll_in_out_count_;
};


#endif //WEBSERV_LIB_ASERVER_H
