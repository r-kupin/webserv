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
#include <csignal>
#include "ServerManager.h"

// Default constructor for the ServerManager class.
ServerManager::ServerManager() {
}

// Destructor for the ServerManager class.
ServerManager::~ServerManager() {}

// Initializes the ServerManager with the provided configuration.
// This includes setting up signal handlers for SIGINT and SIGSTOP to gracefully stop the servers,
// and creating server instances based on the configuration provided.
void ServerManager::Init(const Config &config) {
    // Setup signal handlers to allow for graceful shutdown on SIGINT (Ctrl+C) and SIGSTOP signals.
    signal(SIGINT, Stop);
    signal(SIGSTOP, Stop);

    // Iterate over the server configurations provided in 'config' to create server instances.
    // These instances are stored in the 'servers_' vector for management.
    for (l_sc_c_it it = config.getConstServers().begin(); it != config.getConstServers().end(); ++it) {
        servers_.push_back(Server(*it, is_running_));
    }
    std::cout << "Servers created successfully!" << std::endl;
}

// Starts all the servers managed by this ServerManager.
// This is achieved by creating a thread for each server instance,
// ensuring they can run concurrently without blocking each other.
void ServerManager::Start() {
    // Resize the threads vector to match the number of servers.
    threads_.resize(servers_.size());
    std::cout << "Starting servers... " << std::endl;

    // Create a new thread for each server and start it.
    // If thread creation fails, an error message is printed.
    for (size_t i = 0; i < servers_.size(); ++i) {
        if (pthread_create(&threads_[i], NULL, &StartServer, &servers_[i]) != 0) {
            std::cerr << "Failed to create thread for server " << i << ".\n";
        }
    }
    std::cout << "All servers are up and running!" << std::endl;

    // Wait for all server threads to finish execution.
    for (size_t i = 0; i < threads_.size(); ++i) {
        pthread_join(threads_[i], NULL);
    }
    std::cout << "All servers are stopped" << std::endl;
}

// Static method that serves as the entry point for server threads.
// It casts the void pointer back to a Server pointer and calls the server's Start method.
void *ServerManager::StartServer(void *srv) {
    static_cast<Server*>(srv)->Start();
    return NULL;
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
