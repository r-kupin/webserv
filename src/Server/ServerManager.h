/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/13 12:55:21 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVERMANAGER_H
#define WEBSERV_SERVERMANAGER_H

#include "server/Server.h"
#include "../Config/config/Config.h"

#define NOT_ALL_DATA_WRITTEN_TO_CGI 0
#define CGI_CLOSED_INPUT_FD 1
#define ALL_DATA_SENT_TO_CGI 2
#define NOT_ALL_DATA_READ_FROM_CGI 3
#define CLIENT_CLOSED_CONNECTION_WHILE_CGI_SENDS_DATA 4
#define ALL_READ_ALL_SENT 5

#define CONNECTIONS 2048

class Server;

typedef std::vector<Server>         v_servers;
typedef std::vector<Connection>     v_conn;
typedef std::map<Host, int>         m_host_int;
typedef std::map<int, int>          m_cgifd_to_clientfd;

static volatile bool        is_running_ = true;
static volatile bool        sigpipe_ = false;

class ServerManager {
public:
class ServerManagerException : public std::exception {};

	ServerManager();

	~ServerManager();
//-------------------init-------------------------------------------------------
	void            Init(const Config &config);
	void            CreateEpollInstance();
	void            CreateListeningSocket(const ServerConfiguration &conf);
	void            PresetAddress(addrinfo **addr, const std::string &host,
								  const std::string &port_str);
	int             CreateSocket(addrinfo *res, const std::string &host,
								 const std::string &port_str);
	void            SetSocketOptions(addrinfo *res, int socket);
	void            BindSocket(addrinfo *res, int socket);
	void            ListenSocket(int socket);
	void            AddSocketToEpollInstance(int socket);
//-------------------run--------------------------------------------------------
	void            Start();
	void            EventLoop();
	bool            AddClientToEpoll(int client_sock);
	static void     Signals(int signal);
    void            ReInvokeRequestProcessing(Connection &connection);
    void            IncomingEvent(int socket_fd, uint32_t event);
    void            AcceptNewConnection(int server_socket);
    void            HandleEventsOnExistingConnection(int client_socket);
//-------------------handle-----------------------------------------------------
	const Server    &FindServer(const Connection &connection) const;
	bool            ProcessHeaders(Connection &connection);
	void            CloseConnectionWithLogMessage(int socket,
												  const std::string &msg);
	bool            ProcessBody(Connection &connection);
	bool            Respond(Connection &connection);
	void            CloseTimedOutConnections();
//-------------------cgi--------------------------------------------------------
    void            HandleCGIEvent(int cgi_fd);
    bool            AddCgiToEpoll(int cgi_fd, Connection &connection);
    void            CheckInactiveCGIs();
    void            HandleClosedCGIfd(int terminated_cgi);
//-------------------util-------------------------------------------------------
	void            Cleanup();
	void            PrintEventInfo(int events, int fd, int i) ;
	void            ThrowException(const std::string &msg) const;
	void            Log(const std::string &msg) const;
	bool            IsListeningSocketFd(int socket) const;
	static bool     SetDescriptorNonBlocking(int sockfd) ;
private:
	int                   epoll_fd_;
	v_servers             servers_;
	m_host_int            host_to_socket_; /* quick find-by-host required by CreateListeningSockets */
    m_cgifd_to_clientfd   cgifd_to_cl_sock_;

	v_conn          connections_;
	int             epoll_returns_count_;
	int             epoll_events_count_;
	int             epoll_in_out_count_;
    int             epoll_connection_count_;
	int             active_cgi_processes_;

	Config			config_;
};

#endif //WEBSERV_SERVERMANAGER_H
