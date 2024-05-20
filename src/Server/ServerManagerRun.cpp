/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerRun.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 16:20:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/18 10:43:42 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstring>
#include "ServerManager.h"

/**
 *  Server's main loop
 *  Based around blocking call epoll_wait() which blocks on fd of the
 * server's epoll instance's fd while client wouldn't connect to the one of
 * the defined servers.
 *  If that would happen - epoll_wait will return an event with
 * event.data.fd == socket_ and event.events == EPOLLIN. In this case server
 * accepts connection and adds new socket to the epoll instance, accessible
 * by epoll_fd_.
 *  Otherwise - epoll_wait will return cause to one or more of the already
 * accepted sockets send some data (request) to the server. In this case
 * event.data.fd == already-accepted-socket-fd and
 * event.events & EPOLLIN && event.events & EPOLLOUT. In this case server
 * accepts needs to read incoming data, create the Request, process it and
 * send the response.
 */
void    ServerManager::EventLoop() {
	epoll_event events[MAX_EVENTS];
	int         nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, 1000);

	if (is_running_) {
        if (nfds > 0) {
			// handle reported events
			for (int i = 0; i < nfds; ++i) {
				int         socket_fd = events[i].data.fd;
				uint32_t    event = events[i].events;
				PrintEventInfo(event, socket_fd, i);
				if (!(event & EPOLLERR)) {
					IncomingEvent(socket_fd, event);
				} else if (IsRealError(socket_fd)) {
                    std::cout << "Real error" << std::endl;
					if (connections_[socket_fd].cgi_stdin_fd_ != 0) {
                        CloseCGIfd(connections_[socket_fd].cgi_stdin_fd_);
					} else {
						CloseConnectionWithLogMessage(socket_fd,
													  "client interrupted communication");
					}
				}
			}
		} else {
            bool all = false;
            if (nfds < 0) {
                ThrowException("Epoll wait failed. Shutting down.");
                all = true;
                is_running_ = false;
            }
            CloseConnections(all);
		}
	}
}

void ServerManager::IncomingEvent(int socket_fd, uint32_t event) {
	if (IsListeningSocketFd(socket_fd)) {
		AcceptNewConnection(socket_fd);
	} else if (event & EPOLLIN && event & EPOLLOUT) {
		HandleEventsOnExistingConnection(socket_fd);
	} else if (cgifd_to_cl_sock_.find(socket_fd) != cgifd_to_cl_sock_.end()) {
		HandleCGIEvent(socket_fd);
	}
}

/**
 *  Fulfilling subject's requirement: "A request to your server should never
 * hang forever". Upon accepting() new connection server creates a timestamp.
 * And when server has basically nothing to do - it checks all active
 * connections and closes all expired ones.
 */
void ServerManager::CloseConnections(bool close_all) {
	long time_right_now = Utils::Get().TimeNow();
    std::string msg = close_all ? "Server stopped" : "Connection timed out";
    Log("No events reported: closing connections");
	for (size_t i = 0; i < connections_.size(); i++) {
		if (connections_[i].IsOpen()) {
			long timeout = FindServer(connections_[i]).GetConnectionTimeout();
			if (close_all ||
                connections_[i].HowLongBeingActive(time_right_now) > timeout) {
                if (connections_[i].waiting_for_cgi_) {
                    DetachCGI(connections_[i]);
                    // prepare to respond with 500
                    connections_[i].waiting_for_cgi_ = false;
                    connections_[i].body_done_ = true;
                    connections_[i].location_.SetReturnCode(FAILED_CGI);
                    connections_[i].location_.return_custom_message_ =
                            "Connection closed out while no events were "
                            "reported on the CGI's IO";
                    Respond(connections_[i]);
                }
                CloseConnectionWithLogMessage(i, msg);
			}
		}
	}
}

void ServerManager::DetachCGI(Connection &connection) {
    char c;
    if (read(connection.cgi_stdout_fd_, &c, 1) == -1)
        kill(connection.cgi_pid_, SIGSTOP);
    if (connection.cgi_stdin_fd_ > 0)
        CloseCGIfd(connection.cgi_stdin_fd_);
    CloseCGIfd(connection.cgi_stdout_fd_);
    active_cgi_processes_--;
    closed_cgi_processes_++;
}

void ServerManager::AcceptNewConnection(int server_socket) {
	struct sockaddr_in  client_addr;
	socklen_t           client_len = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr *) &client_addr,
							   &client_len);
	if (client_socket < 0) {
		Log("Error accepting connection!");
	} else if (AddClientToEpoll(client_socket)) {
        // associate client's socket with server's listener
        while (connections_.size() <= (size_t)client_socket) {
            connections_.push_back(Connection(is_running_, active_cgi_processes_));
        }
        connections_[client_socket] = Connection(is_running_,
                                                 client_socket,
                                                 server_socket,
                                                 active_cgi_processes_);
		Log("Accepted client connection from socket " + Utils::NbrToString(client_socket));
	} else {
		Log("Error adding client socket to epoll");
		close(client_socket);
	}
}

/**
 * Add new socket created by accept to epoll instance
 */
bool ServerManager::AddClientToEpoll(int client_sock) {
	epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET;
	event.data.fd = client_sock;
	if (SetDescriptorNonBlocking(client_sock))
		return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_sock, &event) != -1;
	Log("Can't set descriptor " + Utils::NbrToString(client_sock) +
		" to nonblocking mode.");
	return false;
}

bool ServerManager::AddCgiToEpoll(int cgi_fd, Connection &connection) {
	epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET;
	event.data.fd = cgi_fd;
	if (SetDescriptorNonBlocking(cgi_fd)) {
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, cgi_fd, &event) != -1) {
			// map connection with cgi_fd, so when cgi event will be
			// reported, we could find a corresponding connection
			cgifd_to_cl_sock_[cgi_fd] = connection.connection_socket_;
			return true;
		}
	}
	Log("Can't set descriptor " + Utils::NbrToString(cgi_fd) +
		" to nonblocking mode.");
	return false;
}

bool ServerManager::IsRealError(int fd) {
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    return err != 0;
}



