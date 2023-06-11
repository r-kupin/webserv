/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Location.cpp                                       :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/28 11:59:22 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/


#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "Location.h"
#include "../Server/ServerExceptions.h"

const std::map<int, std::string> Location::initializeHttpOkCodes() {
    std::map<int, std::string> codes;
    codes.insert(std::make_pair(100, "Continue"));
    codes.insert(std::make_pair(101, "Switching Protocols"));
    codes.insert(std::make_pair(200, "OK"));
    codes.insert(std::make_pair(201, "Created"));
    codes.insert(std::make_pair(202, "Accepted"));
    codes.insert(std::make_pair(203, "Non-Authoritative Information"));
    codes.insert(std::make_pair(204, "No Content"));
    codes.insert(std::make_pair(205, "Reset Content"));
    codes.insert(std::make_pair(206, "Partial Content"));
    codes.insert(std::make_pair(300, "Multiple Choices"));
    codes.insert(std::make_pair(301, "Moved Permanently"));
    codes.insert(std::make_pair(302, "Found"));
    codes.insert(std::make_pair(303, "See Other"));
    codes.insert(std::make_pair(304, "Not Modified"));
    codes.insert(std::make_pair(305, "Use Proxy"));
    codes.insert(std::make_pair(307, "Temporary Redirect"));
    codes.insert(std::make_pair(308, "Permanent Redirect"));

    return codes;
}

const std::map<int, std::string> Location::kHttpOkCodes =
        Location::initializeHttpOkCodes();

Location::Location()
    : return_code_(0), autoindex_(false) {}

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

bool Location::HasSameAddressAsOneOfSublocationsOf(const Location &rhs) const {
    for (std::_Rb_tree_const_iterator<Location> it = rhs.sublocations_.begin();
         it != rhs.sublocations_.end(); ++it) {
        if (it->HasSameAddressAs(*this))
            return true;
    }
    return false;
}

bool Location::HasSameAddressAs(const Location &rhs) const {
    return address_ == rhs.address_;
}

void Location::AddErrorPages(const v_strings &directive) {
    std::string         address;
    int                 code;

    if (directive.size() > 2) {
        address = *(directive.rbegin());
        for (size_t i = 1; directive[i] != address; ++i) {
            code = std::atoi(directive[i].c_str());
            if (ErrPage::kHttpErrCodes.find(code) == ErrPage::kHttpErrCodes.end()) {
                throw LocationException();
            }
            ErrPage err_page(address, code);
            const std::set<ErrPage>::iterator &iterator =
                    error_pages_.find(err_page);
            if (iterator == error_pages_.end()) {
                error_pages_.insert(err_page);
            } else {
                ErrPage &old = const_cast<ErrPage &>(*iterator);
                old.address_ = address;
            }
        }
    } else {
        throw LocationException();
    }
}

void Location::HandleLocationReturn(const v_strings &directives_) {
    if ((directives_.size() == 2 || directives_.size() == 3) &&
        address_ != "/") { // ?
        return_code_ = atoi(directives_[1].c_str());
        if (kHttpOkCodes.find(return_code_) == kHttpOkCodes.end() &&
            ErrPage::kHttpErrCodes.find(return_code_) ==
            ErrPage::kHttpErrCodes.end()) {
            throw LocationException();
        }
        if (directives_.size() == 3 && return_address_.empty() &&
            directives_[2].find_first_of('/') != std::string::npos) {
            return_address_ = directives_[2];
        }
    } else {
// "only HTTP code and / or address can follow \"return\" keyword, and none can occur repeatedly"
        throw LocationException();
    }
}

const Location & Location::FindSublocationByAddress(
                                            const std::string &address) const {
    if (address == "/")
        return *this;
    LocationByAddress to_find(address);
    std::set<Location>::iterator it = std::find_if(
            sublocations_.begin(),
            sublocations_.end(), to_find);
    if (it == sublocations_.end())
        throw NotFoundException();
    return *it;
}

void Location::HandleRoot(const v_strings &directive) {
    if (directive.size() == 2) {
        root_ = directive[1];
    } else {
        throw LocationException();
    }
}

bool Location::MarkDefined(const std::string &key, bool &flag,
                                      const v_strings &directive) {
    if (directive[0] == key) {
        if (directive.size() < 2) {
            ThrowLocationError("definition has to contain at least key and 1 "
                               "value");
        } else {
            flag = true;
            return true;
        }
    }
    return false;
}

bool Location::UMarkDefined(const std::string &key, bool &flag,
                                       const v_strings &directive) {
    if (directive[0] == key) {
        if (flag) {
            ThrowLocationError("Redefinition of " + key + " is not allowed");
        } else if (directive.size() < 2) {
            ThrowLocationError("definition has to contain at least key and 1 "
                               "value");
        } else {
            flag = true;
            return true;
        }
    }
    return false;
}

void Location::CheckLocationDirectives(std::vector<v_strings> &directives,
                                       ServerConfiguration &sc) {
    bool    root = false, index = false, ret = false, err = false;

    for (size_t i = 0; i < directives.size(); ++i) {
        if (UMarkDefined("root", root, directives[i]))
            HandleRoot(directives[i]);
        if (MarkDefined("index", index, directives[i])) {
            if (address_ == "/") {
                sc.UpdateIndex(directives[i]);
            } else {
                for (size_t j = 1; j < directives[i].size(); ++j) {
                    index_.insert(directives[i][j]);
                }
            }
        }
        if (UMarkDefined("return", ret, directives[i])) {
            HandleLocationReturn(directives[i]);
        }
        if (MarkDefined("error_page", err, directives[i])) {
            AddErrorPages(directives[i]);
        }
    }
}

void Location::ThrowLocationError(const std::string &msg) {
    std::cout << "Syntax error: " + msg << std::endl;
    throw LocationException();
}

LocationByAddress::LocationByAddress(const std::string &targetAddress)
    : targetAddress_(targetAddress) {}

bool LocationByAddress::operator()(const Location &location) const {
    return location.address_ == targetAddress_;
}

std::ostream & Location::RecursivePrint(std::ostream &os, const Location &location,
                                        const std::string &prefix) const {
    os << std::endl << "Localion " << prefix << location.address_ << ":" <<
    std::endl;
    if (!location.error_pages_.empty()) {
        os << prefix << location.address_ << ":\t" << "Error Pages: " <<
        std::endl;
        for (std::_Rb_tree_const_iterator<ErrPage> it =
                location.error_pages_.begin();
             it != location.error_pages_.end(); ++it) {
            os << prefix << location.address_ <<  ":\t\t" << *it << std::endl;
        }
    }
    if (location.return_code_ > 0) {
        os << prefix << location.address_ << ":\t" << "Return Code: " <<
           location.return_code_ << std::endl;
        os << prefix << location.address_ << ":\t" << "Return Address: " <<
           location.return_address_ << std::endl;
    }
    if (!location.index_.empty()) {
        os << prefix << location.address_ << ":\t" << "Index: ";
        for (std::set<std::string>::iterator it = location.index_.begin();
             it != location.index_.end(); ++it) {
            os << *it << " ";
        }
        os << std::endl;
    }
    if (!location.root_.empty())
        os << prefix << location.address_ << ":\t" << "Root: " <<
           location.root_ << std::endl;
    if (location.limit_except_.return_code_ != -1)
        os << prefix << location.address_ << ":\t" << "Limit Except: " <<
           location.limit_except_ << std::endl;
    if (!location.sublocations_.empty()) {
        for (std::_Rb_tree_const_iterator<Location> it =
                location.sublocations_.begin();
             it != location.sublocations_.end(); ++it) {
            RecursivePrint(os, *it, prefix + location.address_);
        }
    }
    return os;
}

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