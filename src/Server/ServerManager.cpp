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
    for (l_sc_c_it it = config.getConstServers().begin();
         it != config.getConstServers().end(); ++it) {
        servers_.push_back(Server(*it));
    }
}

void ServerManager::RunAll() {
    for (l_servers::iterator it = servers_.begin(); it != servers_.end(); ++it) {
        it->Start();
    }
}