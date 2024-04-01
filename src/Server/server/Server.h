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
                    v_c_b &is_running_ref, int epoll_fd);

    bool                        ListensTo(int socket) const;
    const std::string           &GetAddress(int socket) const;
    Location                    ProcessRequest(Connection &connection) const;
    void                        Cleanup(int epoll_fd);

     friend std::ostream        &operator<<(std::ostream &os,
                                            const Server &server);
 protected:
//-------------------initialisation: open sockets, create epoll...--------------
     void                       Init(int epoll_fd);

    void                        PresetAddress(addrinfo **addr,
                                              const std::string &host,
                                              const std::string &port_str);
    int                         CreateSocket(addrinfo *res,
                                             const std::string &host,
                                             const std::string &port_str);
    void                        SetSocketOptions(addrinfo *res, int socket);
    void                        BindSocket(addrinfo *res, int socket);
    void                        ListenSocket(int socket);
    void                        AddSocketToEpollInstance(int socket, int epoll_fd);
//-------------------request server-side processing-----------------------------
    bool                        AccessForbidden(l_loc_c_it found,
                                                Methods method) const;
     bool                       RequestBodyExceedsLimit(l_loc_c_it found,
                                                        const ClientRequest &request) const;
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
                                          const std::string &request_address)const;
//-------------------upload request processing----------------------------------
    int                         UploadFile(ClientRequest &request,
                                           l_loc_c_it found,
                                           int socket) const;
    int                         UploadFromCURL(ClientRequest &request,
                                               const std::string &filename,
                                               int socket) const;
    int                         PerformUpload(const ClientRequest &request,
                                              int socket, int file_fd,
                                              const std::string &delimiter) const;
    bool                        TryCreateOutputFile(const std::string &dir,
                                                    const std::string &filename,
                                                    size_t size) const ;
    void                        HandleUpload(ClientRequest &request,
                                             int socket,
                                             l_loc_c_it &found,
                                             Location &synth) const;
    void                        NoUpoladDataAvailable(int file_fd,
                                                      ssize_t bytes_read) const;
//-------------------misc utils-------------------------------------------------
    void                        Log(const std::string &msg,
                                    std::ostream &os = std::cout) const;
    void                        ThrowException(const std::string &msg,
                                               std::ostream &os = std::cout) const;
private:
    const volatile bool         &is_running_;
    const ServerConfiguration   &config_;
    m_int_str                   srv_sock_to_address_;
 };

#endif //WEBSERV_SERVER_H
