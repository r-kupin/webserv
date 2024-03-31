/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManagerInit.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/31 15:57:14 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ServerManager.h"
#include <csignal>
#include <iostream>
#include <cstring>

// Initializes the ServerManager with the provided configuration.
// This includes setting up signal handlers for SIGINT and SIGSTOP to gracefully stop the servers,
// and creating server instances based on the configuration provided.
void ServerManager::Init(const Config &config) {
    // Setup signal handlers to allow for graceful shutdown on SIGINT (Ctrl+C) and SIGSTOP signals.
    signal(SIGINT, Stop);
    signal(SIGSTOP, Stop);

    CreateEpollInstance();

    // Iterate over the server configurations provided in 'config' to create server instances.
    // These instances are stored in the 'servers_' vector for management.
    for (l_sc_c_it it = config.getConstServers().begin();
         it != config.getConstServers().end(); ++it) {
        servers_.push_back(Server(*it, is_running_, epoll_fd_));
        std::cout << *it << std::endl;
    }

    std::cout << "Servers created successfully!" << std::endl;
}

void ServerManager::CreateEpollInstance() {
    epoll_fd_= epoll_create(1);
    if (epoll_fd_ < 0) {
        std::string err_msg(strerror(errno));
        ThrowException("Failed to Create Epoll Instance : " + err_msg);
    }
    Log("Epoll instance created");
}

v_conn ServerManager::CreateConnections(int n, v_c_b &running) {
    return v_conn();
}
