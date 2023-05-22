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
#include "ConfigSubmodules.h"

ErrPage::ErrPage(const std::string &address, int code)
        : address_(address), code_(code) {}

bool ErrPage::operator<(const ErrPage &rhs) const {
    return code_ < rhs.code_;
}

bool ErrPage::operator==(const ErrPage &rhs) const {
    return address_ == rhs.address_ &&
           code_ == rhs.code_;
}

std::ostream& operator<<(std::ostream &os, const ErrPage &page) {
    os << page.code_ << " " << page.address_;
    return  os;
}

Location::Location()
: return_code_(0), default_index_(true), autoindex_(false) {}

Location::Location(const std::string &address)
: return_code_(0), address_(address) {}

bool Location::operator<(const Location &rhs) const {
    return address_ < rhs.address_;
}

bool Location::operator==(const Location &rhs) const {
    // Compare error_pages_
    if (!(error_pages_ == rhs.error_pages_))
        return false;
    // Compare limit_except_
    if (!(limit_except_ == rhs.limit_except_))
        return false;
    // Compare return_code_
    if (return_code_ != rhs.return_code_)
        return false;
    // Compare index_
    if (index_ != rhs.index_)
        return false;
    // Compare return_address_
    if (return_address_ != rhs.return_address_)
        return false;
    // Compare root_
    if (root_ != rhs.root_)
        return false;
    // Compare address_
    if (address_ != rhs.address_)
        return false;
    // Compare sublocations_
    if (sublocations_ != rhs.sublocations_)
        return false;
    return true;
}

bool Location::HasSameAddress(const Location &rhs) const {
    return address_ == rhs.address_;
}

//
//Location::Location(const std::string &address, const std::string &super_root) {
//
//}

std::ostream& operator<<(std::ostream& os, const Location& location) {
    os << std::endl << "Localion " << location.address_ << ":" << std::endl;
    if (!location.error_pages_.empty()) {
        os << location.address_ << ":\t" << "Error Pages: " << std::endl;
        for (std::_Rb_tree_const_iterator<ErrPage> it =
                location.error_pages_.begin();
              it != location.error_pages_.end(); ++it) {
            os << location.address_ <<  ":\t\t" << *it << std::endl;
        }
    }
    if (location.return_code_ > 0) {
        os << location.address_ << ":\t" << "Return Code: " <<
           location.return_code_ << std::endl;
        os << location.address_ << ":\t" << "Return Address: " <<
           location.return_address_ << std::endl;
    }
    if (!location.index_.empty()) {
        os << location.address_ << ":\t" << "Index: ";
        for (std::set<std::string>::iterator it = location.index_.begin();
             it != location.index_.end(); ++it) {
            os << *it << " ";
        }
        os << std::endl;
    }
    if (!location.root_.empty())
        os << location.address_ << ":\t" << "Root: " <<
                location.root_ << std::endl;
    if (location.limit_except_.return_code_ != -1)
        os << location.address_ << ":\t" << "Limit Except: " <<
                location.limit_except_ << std::endl;
    if (!location.sublocations_.empty()) {
        for (std::_Rb_tree_const_iterator<Location> it =
                location.sublocations_.begin();
             it != location.sublocations_.end(); ++it) {
            os << *it ;
        }
    }
    return os;
}

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

bool ServerConfiguration::operator==(const ServerConfiguration &rhs) const {
    // Compare server properties
    if (port_ != rhs.port_)
        return false;
    if (port_str_ != rhs.port_str_)
        return false;
    if (client_max_body_size_ != rhs.client_max_body_size_)
        return false;
    if (hostname_ != rhs.hostname_)
        return false;
    if (server_names_ != rhs.server_names_)
        return false;

    // Compare root location
    if (!(root_loc_ == rhs.root_loc_))
        return false;

    return true;
}

ServerConfiguration::ServerConfiguration()
: client_max_body_size_(1024),  hostname_("localhost"), root_loc_(Location("/")) {
    root_loc_.root_ = "resources/root_loc_default";
    root_loc_.index_.insert("/htmls/index.html");
    root_loc_.error_pages_.insert(ErrPage("/htmls/404.html", 404));
    root_loc_.error_pages_.insert(ErrPage("/htmls/403.html", 403));
    root_loc_.return_code_ = -1;
    root_loc_.return_address_ = "unspecified";
}
