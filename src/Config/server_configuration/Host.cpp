/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Host.cpp                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/04/06 19:07:41 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "Host.h"
#include "ServerConfiguration.h"

Host::Host(int port, const std::string &name) : port_(port), host_(name) {}

Host::Host(int port) : port_(port), host_("127.0.0.1") {}

Host::Host(const std::string &name) : port_(DEFAULT_PORT), host_(name) {}

Host &Host::operator=(const Host &rhs) {
    if( this != &rhs ) {
        port_ = rhs.port_;
        host_ = rhs.host_;
    }
    return *this;
}

bool operator==(const Host &lhs, const Host &rhs) {
    return lhs.port_ == rhs.port_ &&
           lhs.host_ == rhs.host_;
}

bool Host::operator<(const Host &rhs) const {
    if (port_ != rhs.port_) {
        return port_ < rhs.port_;
    }
    // If ports are equal, compare hosts
    return host_ < rhs.host_;
}

bool Host::operator>(const Host &rhs) const {
    return rhs < *this;
}

bool Host::operator<=(const Host &rhs) const {
    return !(rhs < *this);
}

bool Host::operator>=(const Host &rhs) const {
    return !(*this < rhs);
}
