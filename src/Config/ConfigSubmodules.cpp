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

Limit::Limit() : return_code_(-1) {}

bool Limit::operator==(const Limit &rhs) const {
    return except_ == rhs.except_ &&
           return_code_ == rhs.return_code_;
}

std::ostream &operator<<(std::ostream &os, const Limit &limit) {
    for (std::_Rb_tree_const_iterator<Methods> iterator = limit.except_.begin();
        iterator != limit.except_.end(); ++iterator) {
        os << *iterator << " ";
    }
    os << limit.return_code_;
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
        root_loc_.index_.clear();
        default_index_ = false;
    }
    for (size_t i = 1; i < directive.size(); ++i) {
        root_loc_.index_.insert(directive[i]);
    }
}

void ServerConfiguration::InheritanceErrPagesRoot(const Location &parent,
                                                  Location &start) {
    if (start.root_.empty()) {
        start.root_ = parent.root_;
    }
    if (start.error_pages_.empty()) {
        start.error_pages_ = parent.error_pages_;
    }
	for (std::vector<Location>::iterator it = start.sublocations_.begin();
		 it != start.sublocations_.end(); ++it) {
        InheritanceErrPagesRoot(start, *it);
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
            root_loc_.root_ = directives[i][1];
        } else if (MarkDefined("index", index, directives[i])) {
            UpdateIndex(directives[i]);
        } else if (MarkDefined("error_page", err, directives[i])) {
            root_loc_.AddErrorPages(directives[i]);
        }
    }
    if (!port)
        ThrowServerConfigError("Port needs to be specified explicitly!");
}

ServerConfiguration::ServerConfiguration()
: default_index_(true), default_hostname_(true), client_max_body_size_(1024),
  server_name_("localhost"), root_loc_(Location("/")) {
    root_loc_.root_ = "resources/root_loc_default";
    root_loc_.index_.insert("/htmls/index.html");
    root_loc_.error_pages_.insert(ErrPage("/htmls/404.html", 404));
    root_loc_.error_pages_.insert(ErrPage("/htmls/403.html", 403));
    root_loc_.return_code_ = -1;
    root_loc_.return_address_ = "unspecified";
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
    if (!(root_loc_ == rhs.root_loc_))
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
