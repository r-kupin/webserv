/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/05 16:20:09 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Include necessary headers for signal handling and the server manager's declaration.
#include "ServerManager.h"
#include "connection/Connection.h"
#include <csignal>
#include <unistd.h>		// for fork and pipe
#include <sys/wait.h>	// for waitpid

// Default constructor for the ServerManager class.
ServerManager::ServerManager()
: epoll_returns_count_(0),
epoll_events_count_(0),
epoll_in_out_count_(0),
epoll_connection_count_(0),
active_cgi_processes_(0) {}

// Destructor for the ServerManager class.
ServerManager::~ServerManager() {}

// Starts to handle incoming connections by monitoring of the epoll_fd with
// epoll_wait() block. Runs indefinitely while it receives interruption signal
void ServerManager::Start() {
	Log("Starting servers... ");
	while (is_running_)
		EventLoop();
	Cleanup();
	Log("Server stopped.");
}

// Signal handler method for stopping all servers.
// It checks if the signal received is SIGINT or SIGSTOP, and if so,
// sets a flag to indicate that the servers should stop running.
void ServerManager::Stop(int signal) {
	// if (signal == SIGINT || signal == SIGSTOP) {
	if (signal == SIGINT || signal == SIGTERM) {
		std::cout << "\nStopping servers..." << std::endl;
		is_running_ = false;
	}
}
