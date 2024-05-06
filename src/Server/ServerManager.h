/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/04 17:30:03 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVERMANAGER_H
#define WEBSERV_SERVERMANAGER_H

#include "server/Server.h"
#include "../Config/config/Config.h"

class Server;

typedef std::vector<Server>         v_servers;
typedef std::vector<Connection>     v_conn;
typedef std::map<Host, int>         m_host_int;
typedef std::map<int, Connection*>  m_cgi_fd_conn;

static volatile bool        is_running_ = true;

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
	static void     Stop(int signal);
//-------------------handle-----------------------------------------------------
	const Server    &FindServer(const Connection &connection) const;
	void            AcceptNewConnection(int server_socket);
	void            HandleEventsOnExistingConnection(int client_socket);
	bool            ProcessHeaders(Connection &connection);
	void            CloseConnectionWithLogMessage(int socket,
												  const std::string &msg);
	bool            ProcessBody(Connection &connection);
	bool            Respond(Connection &connection);
	void            CloseTimedOutConnections();
//-------------------util-------------------------------------------------------
	void            Cleanup();
	void            PrintEventInfo(int events, int fd, int i) ;
	void            ThrowException(const std::string &msg) const;
	void            Log(const std::string &msg) const;
	bool            IsListeningSocketFd(int socket) const;
	static bool            SetDescriptorNonBlocking(int sockfd) ;

    bool AddCgiToEpoll(int cgi_fd, Connection *connection);

    void RemoveCGIFromMap(int cgi_fd);

private:
	int             epoll_fd_;
	v_servers       servers_;
	m_host_int      host_to_socket_; /* quick find-by-host required by CreateListeningSockets */
    m_cgi_fd_conn   cgi_fd_to_conn_;

	v_conn          connections_;
	int             epoll_returns_count_;
	int             epoll_events_count_;
	int             epoll_connection_count_;
	int             epoll_in_out_count_;
	int             active_cgi_processes_;

	Config			config_;

    void HandleEventsCGI(int fd);
};

#endif //WEBSERV_SERVERMANAGER_H
