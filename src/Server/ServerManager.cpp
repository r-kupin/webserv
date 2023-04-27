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

#include "ServerManager.h"


ServerManager::ServerManager() {}

ServerManager::ServerManager(const ServerManager &other) {
    (void)other;
}

ServerManager &ServerManager::operator=(const ServerManager &other) {
    if (this == &other)
        return *this;
    return *this;
}

ServerManager::~ServerManager() {}

ServerManager::ServerManager(const Config &config) {
    const v_sconfigs &configs = config.getServers();

    for (size_t i = 0; i < configs.size(); ++i) {
        servers_.push_back(Server(configs[i]));
    }
}

void ServerManager::RunAll() {
    for (size_t i = 0; i < servers_.size(); ++i) {
        servers_[i].Start();
    }
}
