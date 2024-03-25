/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManager.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/11 12:15:17 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include "ServerManager.h"

ServerManager::ServerManager() {
}

ServerManager::~ServerManager() {}

void ServerManager::Init(const Config &config) {
    signal(SIGINT, Stop);
    signal(SIGSTOP, Stop);
    for (l_sc_c_it it = config.getConstServers().begin();
         it != config.getConstServers().end(); ++it) {
        servers_.push_back(Server(*it, is_running_));
    }
    std::cout << "Servers created successfully!" << std::endl;
}

void ServerManager::Start() {
    threads_.resize(servers_.size());
    std::cout << "Starting servers... " << std::endl;
    for (size_t i = 0; i < servers_.size(); ++i) {
        if (pthread_create(&threads_[i],
                           NULL, &StartServer,
                           &servers_[i]) != 0) {
            std::cerr << "Failed to create thread for server " << i << ".\n";
        }
    }
    std::cout << "All servers are up and running!" << std::endl;
    for (size_t i = 0; i < threads_.size(); ++i) {
        pthread_join(threads_[i], NULL);
    }
    std::cout << "All servers are stopped" << std::endl;
}

void *ServerManager::StartServer(void *srv) {
    static_cast<Server*>(srv)->Start();
    return NULL;
}

void ServerManager::Stop(int signal) {
    if (signal == SIGINT || signal == SIGSTOP) {
        std::cout << "\nStopping servers..." << std::endl;
        is_running_ = false;
    }
}