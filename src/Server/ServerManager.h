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
    ServerManager();

    ~ServerManager();
// init
    void            Init(const Config &config);
    static v_conn   CreateConnections(int n, v_c_b &running);
    void            CreateEpollInstance();
// run
    void            Start();
    void            EventLoop();
    int             CheckRequest(int client_sock, int fd);
    bool            AddClientToEpoll(int client_sock);
    static void     Stop(int signal);
// handle

// util
    void            PrintEventInfo(int events, int fd, int i) ;
    void            ThrowException(const std::string &msg) const;
    void            Log(const std::string &msg) const;
    bool            IsSocketFd(int socket_fd) const;
    bool            SetDescriptorNonBlocking(int sockfd) const;
private:
    v_servers       servers_;
    v_conn          connections_;

    int             files_uploaded_;
    long            startup_time_;

    int             epoll_fd_;
    int             epoll_returns_count_;
    int             epoll_events_count_;
    int             epoll_connection_count_;
    int             epoll_in_out_count_;
};


#endif //WEBSERV_SERVERMANAGER_H