/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerRun.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 16:20:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/15 20:07:43 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
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
		epoll_returns_count_++;
		if (nfds == -1) {
			ThrowException("Epoll wait failed. Shutting down.");
		} else if (nfds > 0) {
			// handle reported events
			for (int i = 0; i < nfds; ++i) {
				int         socket_fd = events[i].data.fd;
				uint32_t    event = events[i].events;

				PrintEventInfo(event, socket_fd, i);
				if (!(event & EPOLLERR)) {
					if (IsListeningSocketFd(socket_fd)) {
						AcceptNewConnection(socket_fd);
					} else if (event & EPOLLIN && event & EPOLLOUT) {
						HandleEventsOnExistingConnection(socket_fd);
					} else if ((event & EPOLLIN || event & EPOLLOUT) &&
					cgifd_to_cl_sock_.find(socket_fd) != cgifd_to_cl_sock_.end()) {
						int terminated_cgi;
						if ((terminated_cgi = HandleCGIEvent(socket_fd)) > 0) {
							// delete cgi fd from epoll
							epoll_ctl(terminated_cgi, EPOLL_CTL_DEL, epoll_fd_, NULL);
							// close communication socket
							close(terminated_cgi);
							// remove mapping entry
							CloseConnectionWithLogMessage(cgifd_to_cl_sock_[terminated_cgi],
														  "Cgi transmission ended.");
							cgifd_to_cl_sock_.erase(terminated_cgi);
							active_cgi_processes_--;
						}
					}
				} else {
					HandleFailedClient(socket_fd);
				}
			}
		} else {
			CheckInactiveCGIs();
			CloseTimedOutConnections();
		}
	}
}


void ServerManager::HandleFailedClient(int fd) {
	if (connections_[fd].cgi_stdout_fd_ != 0) {
		std::cout << "X cgi fd: " << connections_[fd].cgi_stdout_fd_ << " socket: "
		<< fd <<
				  std::endl;
		int cgi_fd = connections_[fd].cgi_stdout_fd_;
		// delete cgi fd from epoll
		epoll_ctl(cgi_fd, EPOLL_CTL_DEL, epoll_fd_, NULL);
		// close communication socket
		close(cgi_fd);
		// remove mapping entry
		CloseConnectionWithLogMessage(cgifd_to_cl_sock_[cgi_fd],
									  "Cgi transmission aborted.");
		cgifd_to_cl_sock_.erase(cgi_fd);
		active_cgi_processes_--;
	} else {
		CloseConnectionWithLogMessage(fd, "client interrupted communication");
	}
}

/**
 *  Fulfilling subject's requirement: "A request to your server should never
 * hang forever". Upon accepting() new connection server creates a timestamp.
 * And when server has basically nothing to do - it checks all active
 * connections and closes all expired ones.
 */
void ServerManager::CloseTimedOutConnections() {
	long time_right_now = Utils::Get().TimeNow();

	for (size_t i = 0; i < connections_.size(); i++) {
		if (connections_[i].IsOpen()) {
			long timeout = FindServer(connections_[i]).GetConnectionTimeout();
			if (connections_[i].HowLongBeingActive(time_right_now) > timeout) {
				if (connections_[i].location_.cgi_address_.empty()) {
					CloseConnectionWithLogMessage(i, "Connection timed out");
				}
			}
		}
	}
}

void ServerManager::AcceptNewConnection(int server_socket) {
	struct sockaddr_in  client_addr;
	socklen_t           client_len = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr *) &client_addr,
							   &client_len);
	if (client_socket < 0) {
		Log("Error accepting connection!");
	} else if (AddClientToEpoll(client_socket)) {
		if (connections_.size() > (size_t)client_socket) {
			// associate client's socket with server's listener
			connections_[client_socket] = Connection(is_running_, client_socket,
													 server_socket,
													 active_cgi_processes_);
		} else {
			connections_.push_back(Connection(is_running_, client_socket,
											  server_socket,
											  active_cgi_processes_));
		}
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

void ServerManager::RemoveCGIFromMap(int cgi_fd) {
	cgifd_to_cl_sock_.erase(cgi_fd);
	close(cgi_fd);
}


