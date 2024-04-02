/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/03/18 19:57:03 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVERMANAGER_H
#define WEBSERV_SERVERMANAGER_H

#include "server/Server.h"

typedef std::vector<Server>     v_servers;
typedef std::vector<Connection> v_conn;

static volatile bool        is_running_ = true;

class ServerManager {
public:
class ServerManagerException : public std::exception {};

    ServerManager();

    ~ServerManager();
//-------------------init-------------------------------------------------------
    void            Init(const Config &config);
    void            CreateEpollInstance();
//-------------------run--------------------------------------------------------
    void            Start();
    void            EventLoop();
    bool            AddClientToEpoll(int client_sock);
    static void     Stop(int signal);
//-------------------handle-----------------------------------------------------
    const Server    &FindServerByListeningSocket(int socket) const;
    void            AcceptNewConnection(int server_socket);
    void            HandleEventsOnExistingConnection(int client_socket);
    bool            ProcessHeaders(Connection &connection);
    void            CloseConnectionWithLogMessage(int socket,
                                                  const std::string &msg);
    bool            ProcessBody(Connection &connection, const Server &server);
    void            Respond(Connection &connection);
    void            CloseTimedOutConnections();
//-------------------util-------------------------------------------------------
    void            Cleanup();
    void            PrintEventInfo(int events, int fd, int i) ;
    void            ThrowException(const std::string &msg) const;
    void            Log(const std::string &msg) const;
    bool            IsListeningSocketFd(int socket) const;
    bool            SetDescriptorNonBlocking(int sockfd) const;
private:
    v_servers       servers_;
    v_conn          connections_;

    int             epoll_fd_;
    int             epoll_returns_count_;
    int             epoll_events_count_;
    int             epoll_connection_count_;
    int             epoll_in_out_count_;
};


#endif //WEBSERV_SERVERMANAGER_H