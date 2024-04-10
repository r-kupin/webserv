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
#include "../ServerManager.h"

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
                    v_c_b &is_running_ref,
                    const std::map<Host, int> &all_open_sockets);

    bool                        ListensTo(int socket) const;
    bool                        HasServerName(const std::string &server_name) const;
    std::string                 GetAddress(int socket) const;
    long                        GetConnectionTimeout() const;

    Location                    ProcessRequest(Connection &connection) const;

    friend std::ostream        &operator<<(std::ostream &os,
                                            const Server &server);
protected:
//-------------------request server-side processing-----------------------------
    bool                        AccessForbidden(l_loc_c_it found,
                                                Methods method) const;
    bool                        RequestBodyExceedsLimit(l_loc_c_it found,
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
    void                        Log(const std::string &msg) const;
    void                        Log(const std::string &msg, int listen_sock) const;
    void                        ThrowException(const std::string &msg) const;
    void                        ThrowException(const std::string &msg, int listen_sock) const;
private:
    const volatile bool         &is_running_;
    const ServerConfiguration   &config_;
    std::map<int, Host>         sock_to_host_;
    /* quick find-by-socket required to:
     *  1. find out does this server listens to this socket
     *  2. find address
     * */
 };

#endif //WEBSERV_SERVER_H
