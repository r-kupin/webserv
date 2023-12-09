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
#include "../../Server/ServerExceptions.h"

//-------------------static creation / initialization---------------------------
m_codes_c Location::initializeHttpRedirectCodes() {
    std::map<int, std::string> codes;
    codes.insert(std::make_pair(301, "Moved Permanently"));
    codes.insert(std::make_pair(302, "Found"));
    codes.insert(std::make_pair(303, "See Other"));
    codes.insert(std::make_pair(307, "Temporary Redirect"));
    codes.insert(std::make_pair(308, "Permanent Redirect"));

    return codes;
}

m_codes_c Location::initializeHttpOKCodes() {
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

m_codes_c Location::kHttpOkCodes = Location::initializeHttpOKCodes();
m_codes_c Location::kHttpRedirectCodes = Location::initializeHttpRedirectCodes();

Location::Location(const std::string &address)
	: index_defined_(false),
    return_code_(0),
    address_(address) {
    if (address_.find_first_of('?') != std::string::npos)
        ThrowLocationError("? sign is not allowed in location address");
}

Location::Location()
    : index_defined_(false),
    return_code_(0) {}

Location::Location(const Location& other)
    : error_pages_(other.error_pages_),
    sublocations_(other.sublocations_),
    index_(other.index_),
    index_defined_(other.index_defined_),
    limit_except_(other.limit_except_),
    return_code_(other.return_code_),
    return_address_(other.return_address_),
    root_(other.root_),
    address_(other.address_),
    full_address_(other.full_address_),
    parent_(other.parent_) {}

Location::Location(const std::string &address, l_loc_it parent)
    : index_defined_(false),
    return_code_(0),
    address_(address),
    full_address_(parent->full_address_ + address),
    parent_(parent) {
    if (address_.find_first_of('?') != std::string::npos)
        ThrowLocationError("? sign is not allowed in location address");
}
//-------------------satic utils------------------------------------------------
bool Location::MarkDefined(const std::string &key, bool &flag,
                           const v_str &directive) {
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
                            const v_str &directive) {
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
//-------------------local utils------------------------------------------------
bool is_number(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

bool is_address(const std::string &str) {
    return str.find_first_of('/') != std::string::npos;
}
//-------------------functional stuff-------------------------------------------
l_loc_c_it Location::FindSublocationByAddress(const std::string &address) const {
    if (address == "/")
        return parent_;
    LocationByAddress to_find(address);
    l_loc_c_it it = std::find_if(sublocations_.begin(),
                                 sublocations_.end(),
                                 to_find);
    if (it == sublocations_.end())
        throw NotFoundException();
    return it;
}

LocationByAddress::LocationByAddress(const std::string &targetAddress)
        : targetAddress_(targetAddress) {}

bool LocationByAddress::operator()(const Location &location) const {
    return location.address_ == targetAddress_;
}

bool Location::HasSameAddressAs(const Location &rhs) const {
    return address_ == rhs.address_;
}

bool Location::HasSameAddressAsOneOfSublocationsOf(const Location &rhs) const {
    for (l_loc_c_it it = rhs.sublocations_.begin();
         it != rhs.sublocations_.end(); ++it) {
        if (it->HasSameAddressAs(*this))
            return true;
    }
    return false;
}

bool Location::HasAsSublocation(const Location &location) {
    for (l_loc_it it = sublocations_.begin(); it != sublocations_.end(); ++it) {
        if (location.HasSameAddressAs(*it))
            return true;
    }
    return false;
}

bool Location::HasDefinedLimitExcept() const {
    return !limit_except_.except_.empty();
}
//-------------------setup address----------------------------------------------
void Location::HandleAddress(const std::string &str) {
    const std::string kAddressPrefix = "http://";

    if (is_address(str)) {
        if (return_address_ == "" &&
            str.substr(0, kAddressPrefix.size()) == kAddressPrefix) {
            return_address_ = str;
        } else {
            ThrowLocationError("Return address is already defined or "
                               "misconfigured");
        }
    }
}
//-------------------setup directives handlers----------------------------------
/**
 *  Updates some location parameters based on the directives
 * @param directives
 */
void Location::ProcessDirectives(const std::vector<v_str> &directives) {
    bool root = false;
    bool index = false;
    bool ret = false;
    bool err = false;

    for (size_t i = 0; i < directives.size(); ++i) {
        if (UMarkDefined("root", root, directives[i]))
            HandleRoot(directives[i]);
        if (MarkDefined("index", index, directives[i]))
            HandleIndex(directives[i]);
        if (UMarkDefined("return", ret, directives[i]))
            HandleLocationReturn(directives[i]);
        if (MarkDefined("error_page", err, directives[i]))
            AddErrorPages(directives[i]);
    }
}

void Location::AddErrorPages(const v_str &directive) {
    std::string         address;
    int                 code;

    if (directive.size() > 2) {
        address = *(directive.rbegin());
        for (size_t i = 1; directive[i] != address; ++i) {
            code = std::atoi(directive[i].c_str());
            if (ErrPage::kHttpErrCodes.find(code) ==
                ErrPage::kHttpErrCodes.end()) {
                ThrowLocationError("Error code is wrong");
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
        ThrowLocationError("Error page directive is wrong");
    }
}

void Location::HandleCode(const std::string &str) {
    if (is_number(str)) {
        int code = atoi(str.c_str());
        if ((kHttpOkCodes.find(code) == kHttpOkCodes.end() &&
             ErrPage::kHttpErrCodes.find(code) == ErrPage::kHttpErrCodes.end()) ||
            return_code_ > 100) {
            ThrowLocationError("Return code is wrong");
        }
        return_code_ = code;
    }
}

void Location::HandleIndex(const v_str &directives) {
    if (!index_defined_) {
        index_.clear();
        index_defined_ = true;
    }
    for (size_t j = 1; j < directives.size(); ++j)
        index_.push_back(directives[j]);
}

/**
 * from nginx docs:
 *  	return code [text];
 *  	return code URL;
 *  	return URL;
 *  Stops processing and returns the specified code to a client.
 *  Starting from version 0.8.42, it is possible to specify either a redirect
 * URL (for codes 301, 302, 303, 307, and 308) or the response body text (for
 * other codes). A response body text and redirect URL can contain variables.
 * As a special case, a redirect URL can be specified as a URI local to this
 * server, in which case the full redirect URL is formed according to the
 * request scheme ($scheme) and the server_name_in_redirect and
 * port_in_redirect directives.
 *  In addition, a URL for temporary redirect with the code 302 can be
 * specified as the sole parameter. Such a parameter should start with the
 * “http://”, “https://”, or “$scheme” string. A URL can contain variables.
 *
 *  If return has 1 argument, it is should be a return code or address.
 *  If return has 2 arguments, it is should be a return code and address or
 * custom message !! depending on the return code !!
 *  There can't be more than 2 args, and code can't be the second arg.
 */
void Location::HandleLocationReturn(const v_str &directives_) {
    if (directives_.size() == 2 &&
        (is_number(directives_[1]) || is_address(directives_[1]))) {
        if (is_address(directives_[1])) {
            HandleAddress(directives_[1]);
        } else if (is_number(directives_[1])) {
            HandleCode(directives_[1]);
        } else {
            ThrowLocationError("Return directive is wrong");
        }
    } else if (directives_.size() == 3 && is_number(directives_[1])) {
        HandleCode(directives_[1]);
        if (is_address(directives_[2])) {
            if (kHttpRedirectCodes.find(return_code_) ==
                                                        kHttpRedirectCodes.end())
                ThrowLocationError("Return directive is wrong");
            HandleAddress(directives_[2]);
        } else {
            if (kHttpRedirectCodes.find(return_code_) !=
                                                        kHttpRedirectCodes.end())
                ThrowLocationError("Return directive is wrong");
            return_custom_message_ = directives_[2];
        }
    } else {
        ThrowLocationError("Return directive is wrong");
    }
}

/**
 * from nginx docs:
 *  Syntax: 	root path;
 *  Default: 	root html;
 *  Context: 	http, server, location, if in location
 *
 *  Sets the root directory for requests. For example, with the following
 *  configuration
 *    location /i/ {
 *        root /data/w3;
 *    }
 *  The /data/w3/i/top.gif file will be sent in response to the “/i/top.gif”
 *  request.
 *  The path value can contain variables, except $document_root and
 *  $realpath_root.
 *  A path to the file is constructed by merely adding a URI to the value of
 *  the root directive. If a URI has to be modified, the alias directive
 *  should be used.
 */
void Location::HandleRoot(const v_str &directive) {
    if (directive.size() == 2) {
        root_ = directive[1];
    } else {
        ThrowLocationError("Root directive is wrong");
    }
}
//-------------------setup subcontexts handlers---------------------------------
void Location::HandleLimitExcept(const Node &node) {
    limit_except_.LimExHandleMethods(node.main_);
    limit_except_.LimExHandleDirectives(node.directives_);
}

// todo tests! check root inheritance!
void Location::UpdateSublocations() {
    if (address_ != "/") {
        if (root_.empty())
            root_ = parent_->root_;
        if (root_[root_.size() - 1] == '/')
            root_ = root_.substr(0, root_.size() - 1);
        root_ += address_;
    }
    for (l_loc_it it = sublocations_.begin(); it != sublocations_.end(); ++it) {
        it->UpdateSublocations();
    }
}
//-------------------operator overloads & exceptions----------------------------
void Location::ThrowLocationError(const std::string &msg) {
    std::cout << "Location syntax error: " + msg << std::endl;
    throw LocationException();
}

Location &Location::operator=(const Location &rhs) {
    if (this == &rhs) {
        return *this; // Handle self-assignment
    }
    // Copy the data members from rhs to this object
    error_pages_ = rhs.error_pages_;
    sublocations_ = rhs.sublocations_;
    index_ = rhs.index_;
    index_defined_ = rhs.index_defined_;
    limit_except_ = rhs.limit_except_;
    return_code_ = rhs.return_code_;
    return_address_ = rhs.return_address_;
    root_ = rhs.root_;
    address_ = rhs.address_;
    full_address_ = rhs.full_address_;
    parent_ = rhs.parent_;
    // Return a reference to this object
    return *this;}

bool Location::operator<(const Location &rhs) const {
    return address_ < rhs.address_;
}

bool Location::operator==(const Location &rhs) const {
    if (!(error_pages_ == rhs.error_pages_))
        return false;
    if (!(limit_except_ == rhs.limit_except_))
        return false;
    if (return_code_ != rhs.return_code_)
        return false;
    if (index_ != rhs.index_)
        return false;
    if (return_address_ != rhs.return_address_)
        return false;
    if (root_ != rhs.root_)
        return false;
    if (address_ != rhs.address_)
        return false;
    if (sublocations_ != rhs.sublocations_)
        return false;
    return true;
}

std::ostream &operator<<(std::ostream &os, const Location &location) {
    os << std::endl << "Localion " << location.address_ << ":" <<
       std::endl;
    if (!location.error_pages_.empty()) {
        os << location.full_address_ << ":\t" << "Error Pages: " <<
           std::endl;
        for (s_err_c_it it = location.error_pages_.begin();
             it != location.error_pages_.end(); ++it) {
            os << location.full_address_ <<  ":\t\t" << *it << std::endl;
        }
    }
    if (location.return_code_ > 0) {
        os << location.full_address_ << ":\t" << "Return Code: " <<
           location.return_code_ << std::endl;
        os << location.full_address_ << ":\t" << "Return Address: " <<
           location.return_address_ << std::endl;
    }
    if (!location.index_.empty()) {
        os << location.full_address_ << ":\t" << "Index: ";
        for (l_str_c_it it = location.index_.begin();
             it != location.index_.end(); ++it) {
            os << *it << " ";
        }
        os << std::endl;
    }
    if (!location.root_.empty())
        os << location.full_address_ << ":\t" << "Root: " <<
           location.root_ << std::endl;
    if (location.address_ != "/") {
        os << location.address_ << ":\t" << "Parent: " <<
           location.parent_->address_ << std::endl;
    }
    if (!location.sublocations_.empty()) {
        for (l_loc_c_it it = location.sublocations_.begin();
             it != location.sublocations_.end(); ++it) {
            os << *it;
        }
    }
    return os;
}
