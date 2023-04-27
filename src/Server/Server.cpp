/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Server.cpp                                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:02:49 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include "Server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../Config/ConfigSubmodules.h"


Server::Server() {}

Server::Server(const Server &other)
: config_(other.config_) {}

Server::Server(const ServerConfiguration &config)
: config_(config) {
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {

    }
}

Server &Server::operator=(const Server &other) {
    if (this == &other)
        return *this;
    return *this;
}

Server::~Server() {}

void Server::Start() {
    std::cout << "started server at " << config_.port_ << " port" << std::endl;
}
