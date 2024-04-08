/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerConfiguration.cpp                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/22 16:59:53 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <algorithm>
#include <iostream>
#include <sstream>

#include "ServerConfiguration.h"

ServerConfiguration::ServerConfiguration()
: default_host_(true) {
    hosts_.insert(Host(DEFAULT_PORT, "localhost"));
    keepalive_timeout_ = 60000;
    Location root_loc("/");
    root_loc.root_ = kDefaultResources;
    root_loc.return_code_ = 0;
    root_loc.return_internal_address_ = "";
    root_loc.full_address_ = "";
    locations_.push_back(root_loc);
    locations_.begin()->parent_ = locations_.begin();
}

ServerConfiguration::ServerConfiguration(const ServerConfiguration &other)
: default_host_(other.default_host_),
    hosts_(other.hosts_),
    server_names_(other.server_names_),
    locations_(other.locations_),
    keepalive_timeout_(other.keepalive_timeout_) {}
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
//-------------------server level
    bool srv_name = false;
    bool port = false;
//-------------------root location level
    bool cl_max_bd_size = false;
    bool err = false;
    bool index = false;
    bool root = false;
    bool uploads = false;

    if (directives.empty())
        ThrowServerConfigError("Server block can't be empty!");
    try {
        for (size_t i = 0; i < directives.size(); i++) {
            if (UMarkDefined("server_name", srv_name, directives[i])) {
                HandleServerNames(directives[i]);
            } else if (UMarkDefined("keepalive_timeout", srv_name,
                                    directives[i])) {
                HandleKeepaliveTimeout(directives[i]);
            } else if (MarkDefined("listen", port, directives[i])) {
                HandleHost(directives[i]);
            } else if (UMarkDefined("client_max_body_size", cl_max_bd_size,
                                    directives[i])) {
                GetRoot().HandleClientMaxBodySize(directives[i]);
            } else if (UMarkDefined("root", root, directives[i])) {
                GetRoot().HandleRoot(directives[i]);
            } else if (MarkDefined("index", index, directives[i])) {
                GetRoot().HandleIndex(directives[i]);
            } else if (MarkDefined("error_page", err, directives[i])) {
                GetRoot().AddErrorPages(directives[i]);
            } else if (UMarkDefined("upload_store", uploads, directives[i])) {
                GetRoot().SetUploadsDirectory(directives[i]);
            }
        }
    } catch (const Location::LocationException &) {
        ThrowServerConfigError("Root-related directives are misconfigured");
    }
    if (!port)
        ThrowServerConfigError("Port needs to be specified explicitly!");
}

void ServerConfiguration::HandleKeepaliveTimeout(const v_str &directive) {
    if (directive.size() == 2) {
        keepalive_timeout_ = atoi(directive[1].c_str());
        if (keepalive_timeout_ > 0)
            return;
    }
    ThrowServerConfigError("keepalive_timeout directive is wrong");
}

void ServerConfiguration::HandleServerNames(const v_str &directive) {
    if (directive.size() > 1) {
        for (v_str_c_it it = directive.begin() + 1; it != directive.end(); ++it) {
            server_names_.insert(*it);
        }
        return;
    }
    ThrowServerConfigError("server_name directive is wrong");
}

// todo tests
// todo probably need to insert as server_name as well
/**
 *  The listen directive specifies the IP address and port on which Nginx will
 * listen for incoming connections.
 * It defines the socket on which the server will accept client requests.
 *  Valid values are:
 *  - port (default "localhost" address will be applied)
 *  - host (default port 4280 will be applied)
 *  - host:port
 *  Host may be IPv4 address (127.0.0.1) or DNS name (localhost)
 */
void ServerConfiguration::HandleHost(const v_str &directive) {
    if (default_host_)
        hosts_.clear();
    default_host_ = false;
    if (directive.size() != 2)
        ThrowServerConfigError("listen directive is wrong");

    if (Utils::IsPositiveNumber(directive[1])) {
        // port only
        hosts_.insert(Host(std::atoi(directive[1].c_str())));
    } else {
        std::string address = directive[1];
        std::string port = Utils::NbrToString(DEFAULT_PORT);

        if (directive[1].find_first_of(':') != std::string::npos) {
            // address with port
            std::istringstream  iss(directive[1]);
            // separate address and port
            if (!std::getline(iss, address, ':') ||
                !std::getline(iss, port) || !Utils::IsPositiveNumber(port)) {
                // line has semicolon, but something is missing or port is negative
                ThrowServerConfigError("listen directive is wrong");
            }
        }

        address = Utils::LookupDNS(address);
        if (!address.empty()) {
            hosts_.insert(Host(std::atoi(port.c_str()), address));
        } else {
            ThrowServerConfigError("address DNS lookup failed");
        }
    }
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

l_loc_it    ServerConfiguration::GetRootIt() {
    return locations_.begin();
}

l_loc_c_it ServerConfiguration::GetConstRootIt() const {
    return locations_.begin();
}

const s_str &ServerConfiguration::GetServerNames() const {
    return server_names_;
}

const l_loc &ServerConfiguration::GetLocations() const {
    return locations_;
}

long ServerConfiguration::GetKeepaliveTimeout() const {
    return keepalive_timeout_;
}

const s_hosts &ServerConfiguration::GetHosts() const {
    return hosts_;
}

bool        ServerConfiguration::operator==(
                                        const ServerConfiguration &rhs) const {
    if (server_names_ != rhs.server_names_)
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
    server_names_ = rhs.server_names_;
    locations_ = rhs.locations_;
    keepalive_timeout_ = rhs.keepalive_timeout_;
    hosts_ = rhs.hosts_;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const ServerConfiguration &config) {
    os << "server names: " << "\n";
    for (s_str ::const_iterator it = config.server_names_.begin();
         it != config.server_names_.end(); ++it) {
        os << "\t" << *it << std::endl;
    }
    os << config.locations_.front() << "\n";
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
