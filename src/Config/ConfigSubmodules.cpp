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
#include "ConfigSubmodules.h"
#include "ConfigExceptions.h"

Limit::Limit() : deny_all_(false), allow_all_(true) {}

bool Limit::operator==(const Limit &rhs) const {
    return except_ == rhs.except_;
}

std::ostream &operator<<(std::ostream &os, const Limit &limit) {
    for (std::_Rb_tree_const_iterator<Methods> iterator = limit.except_.begin();
        iterator != limit.except_.end(); ++iterator) {
        os << *iterator << " ";
    }
    return os;
}

void ServerConfiguration::UpdateHostname(const v_strings &directives) {
    // TODO server names - hostnames ....
    if (default_hostname_) {
        server_names_.clear();
        default_hostname_ = false;
    }
    for (size_t i = 1; i < directives.size(); ++i)
        server_names_.insert(directives[i]);
}

void ServerConfiguration::UpdateIndex(const v_strings &directive) {
    if (default_index_) {
        locations_.begin()->index_.clear();
        default_index_ = false;
    }
    for (size_t i = 1; i < directive.size(); ++i) {
        locations_.begin()->index_.insert(directive[i]);
    }
}

void ServerConfiguration::InheritanceErrPagesRoot(l_it parent,
                                                  std::list<Location> &kids) {
    for (l_it it = kids.begin(); it != kids.end(); ++it) {
        if (it->root_.empty())
            it->root_ = parent->root_;
        if (it->address_ != "/")
            it->root_ = it->root_.substr(0, it->root_.find_last_of('/')) +
                    it->address_ + "/";
        if (it->error_pages_.empty())
            it->error_pages_ = parent->error_pages_;
        if (!it->sublocations_.empty())
            InheritanceErrPagesRoot(it, it->sublocations_);
    }
}

void
ServerConfiguration::CheckServerDirectives(std::vector<v_strings> &directives) {
    bool srv_name = false;
    bool cl_max_bd_size = false;
    bool err = false;
    bool index = false;
    bool root = false;
    bool port = false;

    for (size_t i = 0; i < directives.size(); i++) {
        if (MarkDefined("server_name", srv_name, directives[i])) {
            UpdateHostname(directives[i]);
        } else if (UMarkDefined("listen", port, directives[i])) {
            port_ = atoi(directives[i][1].c_str());
            port_str_ = directives[i][1];
        } else if (UMarkDefined("client_max_body_size", cl_max_bd_size,
                                directives[i])) {
            client_max_body_size_ = atoi(directives[i][1].c_str());
        } else if (UMarkDefined("root", root, directives[i])) {
            locations_.begin()->root_ = directives[i][1];
        } else if (MarkDefined("index", index, directives[i])) {
            UpdateIndex(directives[i]);
        } else if (MarkDefined("error_page", err, directives[i])) {
            locations_.begin()->AddErrorPages(directives[i]);
        }
    }
    if (!port)
        ThrowServerConfigError("Port needs to be specified explicitly!");
}

ServerConfiguration::ServerConfiguration()
: default_index_(true), default_hostname_(true), client_max_body_size_(1024),
  server_name_("localhost") {
    Location root_loc("/");
    root_loc.root_ = "resources/root_loc_default";
    root_loc.index_.insert("/htmls/index.html");
    root_loc.error_pages_.insert(ErrPage("/htmls/404.html", 404));
    root_loc.error_pages_.insert(ErrPage("/htmls/403.html", 403));
    root_loc.return_code_ = -1;
    root_loc.return_address_ = "unspecified";
    root_loc.parent_ = locations_.end();
    locations_.push_back(root_loc);
}

bool ServerConfiguration::operator==(const ServerConfiguration &rhs) const {
    // Compare server properties
    if (port_ != rhs.port_)
        return false;
    if (port_str_ != rhs.port_str_)
        return false;
    if (client_max_body_size_ != rhs.client_max_body_size_)
        return false;
    if (server_name_ != rhs.server_name_)
        return false;
    if (server_names_ != rhs.server_names_)
        return false;

    // Compare root location
    if (!(locations_ == rhs.locations_))
        return false;

    return true;
}

bool ServerConfiguration::MarkDefined(const std::string &key, bool &flag,
                                      const v_strings &directive) {
    if (directive[0] == key && directive.size() > 1) {
        flag = true;
        return true;
    }
    return false;}

bool ServerConfiguration::UMarkDefined(const std::string &key, bool &flag,
                                       const v_strings &directive) {
    if (directive[0] == key && directive.size() > 1) {
        if (flag)
            ThrowServerConfigError(
                    "Multiple occurrence of the unique directive!");
        flag = true;
        return true;
    }
    return false;
}

void    ServerConfiguration::ThrowServerConfigError(const std::string &msg) {
    std::cout << "ServerConfigError: " + msg << std::endl;
    throw ConfigFileSyntaxError();
}

ServerConfiguration::ServerConfiguration(const ServerConfiguration &other)
: default_index_(other.default_index_),
  default_hostname_(other.default_hostname_),
  port_(other.port_),
  port_str_(other.port_str_),
  client_max_body_size_(other.client_max_body_size_),
  server_name_(other.server_name_),
  server_names_(other.server_names_),
  locations_(other.locations_) {}

ServerConfiguration &
ServerConfiguration::operator=(const ServerConfiguration &rhs) {
    if (this == &rhs) {
        // Self-assignment, no need to do anything
        return *this;
    }

    // Copy data members from rhs to this object
    default_index_ = rhs.default_index_;
    default_hostname_ = rhs.default_hostname_;
    port_ = rhs.port_;
    port_str_ = rhs.port_str_;
    client_max_body_size_ = rhs.client_max_body_size_;
    server_name_ = rhs.server_name_;
    server_names_ = rhs.server_names_;
    locations_ = rhs.locations_;

    // Return the updated object
    return *this;
}
