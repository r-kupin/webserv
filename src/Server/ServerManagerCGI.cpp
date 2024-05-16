/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerCGI.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 16:20:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/16 13:46:47 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include "ServerManager.h"
#include "server/ServerExceptions.h"

/**
 * At this point there might be cgi connections on which events weren't
 * reported, because cgi process ended and / or closed the connection. In
 * this method we are looping across all connections:
 * - If connection is waiting for CGI process to end - we'll try to read a  bit
 * from cgi_stdout_fd ("handle" event that didn't happen).
 *      - If read returns -1 - cgi process is still running and might send us
 *      something.
 *      - If read returns 0 - cgi process is done, and we need to close the
 *      connection.
 * - If connection is NOT waiting for CGI process - it means that the limit
 * of processes was reached at the time of initialisation of this connection,
 * and if current amount of processes is less than MAX_CGI_PROCESSES we'll 
 * launch it 
 */
// void ServerManager::CheckInactiveCGIs() {
// 	// no events were reported during epoll_wait timeout:
// 	// check all existing connections and close expired ones
// 	// 0 - no cgi connections
// 	// -1 - no terminated cgi connections
// 	// > 0 - terminated one
// 	int terminated_cgi = 0;
// 	for (std::map<int, int>::iterator it = cgifd_to_cl_sock_.begin();
// 		 it != cgifd_to_cl_sock_.end(); ++it) {
// 		std::cout << "cgi fd: " << it->first << " socket: " << it->second <<
// 		std::endl;
// 		// There are some CGI connections
// 		Connection &connection = connections_[it->second];
// 		if (connection.waiting_for_cgi_) {
// 			if ((terminated_cgi = HandleCGIEvent(connection)) != -1) {
// 				// can't erase element from a map in wich we are iterating.
// 				// break and relaunch
// 				break;
// 			}
// 		} else if (active_cgi_processes_ < MAX_CGI_PROCESSES) {
// 			ReInvokeRequestProcessing(connection);
// 		}
// 	}
// 	if (terminated_cgi > 0) {
// 		// found map entry with a connection that ended it's communication with a
// 		// CGI process, and needs to be removed to avoid further attempts to
// 		// access the FD associated with a dead cgi process
// 		HandleTerminatedCGIProcess(terminated_cgi);
// 		// Check the rest of them
// 		CheckInactiveCGIs();
// 	}
// }

// Check and handle inactive or terminated CGI processes
void ServerManager::CheckInactiveCGIs() {
	std::map<int, int>::iterator it = cgifd_to_cl_sock_.begin();
	while (it != cgifd_to_cl_sock_.end()) {
		Connection &connection = connections_[it->second];
		if (connection.waiting_for_cgi_) {
			int cgi_status = HandleCGIEvent(connection);
			if (cgi_status == 0) {
				cgifd_to_cl_sock_.erase(it++);  // Safely erase while iterating
			} else {
				++it;
			}
		} else {
			++it; // Continue if not waiting for CGI
		}
	}
}


// void ServerManager::HandleTerminatedCGIProcess(int terminated_cgi) {
// 	// delete cgi fd from epoll
// 	epoll_ctl(terminated_cgi, EPOLL_CTL_DEL, epoll_fd_, NULL);
// 	// close communication socket
// 	close(terminated_cgi);
// 	// remove mapping entry
// 	CloseConnectionWithLogMessage(cgifd_to_cl_sock_[terminated_cgi],
// 								  "Cgi transmission ended.");
// 	cgifd_to_cl_sock_.erase(terminated_cgi);
// 	active_cgi_processes_--;
// }

// Handles terminated CGI processes, cleaning up file descriptors and epoll entries
void ServerManager::HandleTerminatedCGIProcess(int cgi_fd) {
	epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, cgi_fd, NULL); // Remove from epoll
	close(cgi_fd); // Close CGI file descriptor
	cgifd_to_cl_sock_.erase(cgi_fd); // Erase from tracking map
	Log("CGI process terminated and cleaned up.");
}


void ServerManager::ReInvokeRequestProcessing(Connection &connection) {
	const Server &server = FindServer(connection);
	connection.location_ = server.ProcessRequest(connection);
}

int ServerManager::HandleCGIEvent(int cgi_stdout_fd) {
	if (cgifd_to_cl_sock_.find(cgi_stdout_fd) != cgifd_to_cl_sock_.end()) {
		int clients_socket = cgifd_to_cl_sock_.find(cgi_stdout_fd)->second;
		Connection &conection = connections_[clients_socket];
		int handling_result = HandleCGIEvent(conection);
		return handling_result;
	}
	Log("Pair CGI_stdout_fd - Client socket not found");
	return -1;
}

// int ServerManager::HandleCGIEvent(Connection &connection) {
// 	try {
// 		const Server &server = FindServer(connection);
// 		if(!server.HandleCGIinput(connection))
// 			return connection.cgi_stdout_fd_;
// 	} catch (const EwouldblockEagainUpload &) {
// 		Log("Read all available data, but cgi transmission is incomplete. "
// 			"We'll come back later. Maybe.");
// 	} catch (...) {
// 		// Reset connection state for a particular client (keep client's fd)
// 		int cgi_stdout_fd = connection.cgi_stdout_fd_;
// 		return cgi_stdout_fd;
// 	}
// 	return -1;
// }

// Handle incoming or outgoing CGI events
int ServerManager::HandleCGIEvent(Connection &connection) {
	try {
		const Server &server = FindServer(connection);
		if (!server.HandleCGIinput(connection)) {
			return -1; // Continue if more data is expected
		}
		if (!server.SendDataToCGI(connection, "Data to send")) {
			Log("Failed to send data to CGI");
			return -1;
		}
		return 0; // Indicate CGI processing complete
	} catch (...) {
		Log("Unexpected error during CGI handling.");
		return -1;
	}
}

// Example processing of CGI output for simplicity
bool Server::ProcessCGIOutput(Connection &connection) const {
	// Convert vector<char> to string for logging
	std::string output(connection.buffer_.begin(), connection.buffer_.end());
	// Simple response verification and sending logic
	Log("Processing CGI output: " + output);
	send(connection.connection_socket_, connection.buffer_.data(), connection.buffer_.size(), 0);
	connection.buffer_.clear();
	return true;
}

