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

#include "ConfigSubmodules.h"

bool ErrPage::operator==(const ErrPage &rhs) const {
    return code_ == rhs.code_;
}

ErrPage::ErrPage(const std::string &address, int code)
        : address_(address), code_(code) {}

bool ErrPage::operator<(const ErrPage &rhs) const {
    return code_ < rhs.code_;
}

bool ErrPage::operator>(const ErrPage &rhs) const {
    return rhs < *this;
}

bool ErrPage::operator<=(const ErrPage &rhs) const {
    return !(rhs < *this);
}

bool ErrPage::operator>=(const ErrPage &rhs) const {
    return !(*this < rhs);
}


bool Location::operator==(const Location &rhs) const {
    return address_ == rhs.address_;
}

Location::Location()
        : return_code_(0) {}

Location::Location(const std::string &address) : address_(address) {}


bool ServerConfiguration::operator==(const ServerConfiguration &rhs) const {
    return port_ == rhs.port_;
}

ServerConfiguration::ServerConfiguration()
: client_max_body_size_(1024),  hostname_("localhost"),
    explicit_default_location_set_(false) {
    Location def = Location("/");
    def.root_ = "resources/default";
    def.index_.push_back(def.root_ + "/htmls/index.html");
    def.error_pages_.insert(ErrPage(def.root_ + "/htmls/404.html", 404));
    def.error_pages_.insert(ErrPage(def.root_ + "/htmls/403.html", 403));
    def.return_code_ = -1;
    def.return_address_ = "unspecified";
    locations_.push_back(def);
}
