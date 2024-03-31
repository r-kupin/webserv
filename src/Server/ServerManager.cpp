/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/03/28 18:56:30 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Include necessary headers for signal handling and the server manager's declaration.
#include "ServerManager.h"
#include <csignal>

// Default constructor for the ServerManager class.
ServerManager::ServerManager()
: startup_time_(Utils::TimeNow()) {}

// Destructor for the ServerManager class.
ServerManager::~ServerManager() {}

// Starts all the servers managed by this ServerManager.
// This is achieved by creating a thread for each server instance,
// ensuring they can run concurrently without blocking each other.
void ServerManager::Start() {
    Log("Starting servers... ");
    while (is_running_)
        EventLoop();
//    Cleanup();
    Log("Server stopped.");
}

// Signal handler method for stopping all servers.
// It checks if the signal received is SIGINT or SIGSTOP, and if so,
// sets a flag to indicate that the servers should stop running.
void ServerManager::Stop(int signal) {
    if (signal == SIGINT || signal == SIGSTOP) {
        std::cout << "\nStopping servers..." << std::endl;
        is_running_ = false;
    }
}
