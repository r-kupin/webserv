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
#include "ServerConfiguration.h"

ServerConfiguration::ServerConfiguration() {
    server_names_.insert("localhost");
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
: ports_(other.ports_),
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
                HandlePort(directives[i]);
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

void ServerConfiguration::HandlePort(const v_str &directive) {
    if (directive.size() == 2) {
        int port = atoi(directive[1].c_str());
        if (port >= 0) {
            ports_.insert(port);
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

l_loc_it    ServerConfiguration::GetRootIt() {
    return locations_.begin();
}

l_loc_c_it ServerConfiguration::GetConstRootIt() const {
    return locations_.begin();
}

const std::set<int> & ServerConfiguration::GetPorts() const {
    return ports_;
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

bool        ServerConfiguration::operator==(
                                        const ServerConfiguration &rhs) const {
    if (ports_ != rhs.ports_)
        return false;
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
    ports_ = rhs.ports_;
    server_names_ = rhs.server_names_;
    locations_ = rhs.locations_;
    keepalive_timeout_ = rhs.keepalive_timeout_;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const ServerConfiguration &config) {
    os << "addresses: " << "\n";
    for (s_str_c_it it_hn = config.server_names_.begin();
         it_hn != config.server_names_.end(); ++it_hn) {
        for (s_int_c_it it_p = config.ports_.begin();
             it_p != config.GetPorts().end(); ++it_p) {
            os << "\t" << *it_hn << ":" << *it_p << "\n";
        }
    }
    os << config.locations_.front() << "\n";
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

