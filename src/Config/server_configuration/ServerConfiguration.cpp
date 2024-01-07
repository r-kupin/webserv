/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSubmodules.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/22 16:59:53 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <algorithm>
#include <iostream>
#include "ServerConfiguration.h"

ServerConfiguration::ServerConfiguration()
: client_max_body_size_(0),
server_name_("localhost") {
    Location root_loc("/");
    root_loc.root_ = kDefaultResources;
    root_loc.return_code_ = 0;
    root_loc.return_internal_address_ = "";
    root_loc.full_address_ = "";
    locations_.push_back(root_loc);
    locations_.begin()->parent_ = locations_.begin();
}

ServerConfiguration::ServerConfiguration(const ServerConfiguration &other)
: port_(other.port_),
client_max_body_size_(other.client_max_body_size_),
server_name_(other.server_name_),
locations_(other.locations_) {}
//-------------------satic utils------------------------------------------------
bool        ServerConfiguration::MarkDefined(const std::string &key,
                                             bool &flag,
                                             const v_str &directive) {
    if (directive[0] == key && directive.size() > 1) {
        flag = true;
        return true;
    }
    return false;
}

bool        ServerConfiguration::UMarkDefined(const std::string &key, bool &flag,
                                       const v_str &directive) {
    if (directive[0] == key && directive.size() > 1) {
        if (flag)
            ThrowServerConfigError(
                    "Multiple occurrence of the unique directive!");
        flag = true;
        return true;
    }
    return false;
}
//-------------------setup directives handlers----------------------------------
void        ServerConfiguration::ProcessDirectives(
                                              std::vector<v_str> &directives) {
    bool srv_name = false;
    bool cl_max_bd_size = false;
    bool err = false;
    bool index = false;
    bool root = false;
    bool port = false;

    if (directives.empty())
        ThrowServerConfigError("Server block can't be empty!");
    try {
        for (size_t i = 0; i < directives.size(); i++) {
            if (UMarkDefined("server_name", srv_name, directives[i])) {
                server_name_ = directives[i][1];
            } else if (UMarkDefined("listen", port, directives[i])) {
                HandlePort(directives[i]);
            } else if (UMarkDefined("client_max_body_size", cl_max_bd_size,
                                    directives[i])) {
                HandleClientMaxBodySize(directives[i]);
            } else if (UMarkDefined("root", root, directives[i])) {
                GetRoot().HandleRoot(directives[i]);
            } else if (MarkDefined("index", index, directives[i])) {
                GetRoot().HandleIndex(directives[i]);
            } else if (MarkDefined("error_page", err, directives[i])) {
                GetRoot().AddErrorPages(directives[i]);
            }
        }
    } catch (const Location::LocationException &) {
        ThrowServerConfigError("Root-related directives are misconfigured");
    }
    if (!port)
        ThrowServerConfigError("Port needs to be specified explicitly!");
}

void ServerConfiguration::HandleClientMaxBodySize(const v_str &directive) {
    if (directive.size() == 2) {
        int size = atoi(directive[1].c_str());
        if (size >= 0) {
            client_max_body_size_ = size;
            return;
        }
    }
    ThrowServerConfigError("client_max_body_size_ directive is wrong");
}

void ServerConfiguration::HandlePort(const v_str &directive) {
    if (directive.size() == 2) {
        int port = atoi(directive[1].c_str());
        if (port >= 0) {
            port_ = port;
            return;
        }
    }
    ThrowServerConfigError("port directive is wrong");
}

//-------------------operator overloads & exceptions----------------------------
void        ServerConfiguration::ThrowServerConfigError(const std::string &msg) {
    std::cout << "Server config syntax error: " + msg << std::endl;
    throw ServerConfigurationException();
}

Location        &ServerConfiguration::GetRoot() {
    return locations_.front();
}

const Location  &ServerConfiguration::GetConstRoot() const {
    return locations_.front();
}

int ServerConfiguration::GetClientMaxBodySize() const {
    return client_max_body_size_;
}

l_loc_it    ServerConfiguration::GetRootIt() {
    return locations_.begin();
}

l_loc_c_it ServerConfiguration::GetConstRootIt() const {
    return locations_.begin();
}

int ServerConfiguration::GetPort() const {
    return port_;
}

const std::string &ServerConfiguration::GetServerName() const {
    return server_name_;
}

const l_loc &ServerConfiguration::GetLocations() const {
    return locations_;
}

bool        ServerConfiguration::operator==(
                                        const ServerConfiguration &rhs) const {
    if (port_ != rhs.port_)
        return false;
    if (client_max_body_size_ != rhs.client_max_body_size_)
        return false;
    if (server_name_ != rhs.server_name_)
        return false;
    if (!(locations_ == rhs.locations_))
        return false;
    return true;
}

ServerConfiguration &ServerConfiguration::operator=(
                                            const ServerConfiguration &rhs) {
    if (this == &rhs) {
        return *this;
    }
    port_ = rhs.port_;
    client_max_body_size_ = rhs.client_max_body_size_;
    server_name_ = rhs.server_name_;
    locations_ = rhs.locations_;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const ServerConfiguration &config) {
    os << "hostname: " << config.GetServerName() << "\n";
    os << "port: " << config.GetPort() << "\n";
    if (config.GetClientMaxBodySize()) {
        os << "client_max_body_size_: " << config.GetClientMaxBodySize() << "\n";
    }
    os << config.GetConstRoot() << "\n";
    os << std::endl;
    return os;
}

Srch_Res::LocSearchResult(const l_loc_it &location,
                          const std::string &status,
                          const std::string &fullAddress,
                          const std::string &leftowerAddress)
        : location_(location),
        status_(status),
        full_address_(fullAddress),
        leftower_address_(leftowerAddress) {}

Srch_c_Res::LocConstSearchResult(const l_loc_c_it &location,
                                 const std::string &status,
                                 const std::string &fullAddress,
                                 const std::string &leftowerAddress)
        : location_(location),
        status_(status),
        full_address_(fullAddress),
        leftower_address_(leftowerAddress) {}

