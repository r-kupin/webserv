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

//-------------------static creation / initialization---------------------------
const m_codes Location::initializeHttpRedirectCodes() {
    m_codes codes;
    codes.insert(std::make_pair(301, "Moved Permanently"));
    codes.insert(std::make_pair(302, "Found"));
    codes.insert(std::make_pair(303, "See Other"));
    codes.insert(std::make_pair(307, "Temporary Redirect"));
    codes.insert(std::make_pair(308, "Permanent Redirect"));

    return codes;
}

const m_codes Location::initializeHttpOKCodes() {
    m_codes codes;
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

const m_codes Location::kHttpOkCodes = Location::initializeHttpOKCodes();
const m_codes Location::kHttpRedirectCodes = Location::initializeHttpRedirectCodes();

Location::Location()
    : has_own_index_defined_(false),
      return_code_(0),
      ghost_(false) {}

// link to parent? indexes?
Location::Location(bool ghost, const std::string &address)
    : full_address_(HandleAddressInConstructor(address)),
    address_(GetParticularAddress(address)),
    ghost_(ghost) {}

Location::Location(const Location& other)
    : error_pages_(other.error_pages_),
      sublocations_(other.sublocations_),
      has_own_index_defined_(other.has_own_index_defined_),
      index_defined_in_parent_(other.index_defined_in_parent_),
      own_index_(other.own_index_),
      limit_except_(other.limit_except_),
      return_code_(other.return_code_),
      return_internal_address_(other.return_internal_address_),
      return_external_address_(other.return_external_address_),
      return_custom_message_(other.return_custom_message_),
      root_(other.root_),
      full_address_(other.full_address_),
      address_(other.address_),
      body_file_(other.body_file_),
      parent_(other.parent_),
      ghost_(other.ghost_) {}

Location::Location(const std::string &address)
    : has_own_index_defined_(false),
      index_defined_in_parent_(false),
      return_code_(0),
      full_address_(HandleAddressInConstructor(address)),
      address_(GetParticularAddress(address)),
      ghost_(false) {}

Location::Location(const std::string &address, l_loc_it parent)
    : has_own_index_defined_(false),
    index_defined_in_parent_(false),
    return_code_(0),
    full_address_(HandleAddressInConstructor(address)),
    address_(GetParticularAddress(address)),
    parent_(parent),
    ghost_(false) {
    if (parent->index_defined_in_parent_ ||
        parent->has_own_index_defined_) {
        index_defined_in_parent_ = true;
    }
    if ((parent->address_ != "/") &&
        (parent->full_address_ + address_ != full_address_ )) {
            ThrowLocationException(
                "Full location address should start with parent "
                "full address");
    }
}

Location Location::GhostLocation(const std::string &address) {
    return Location(true, address);
}
//
//Location Location::RootLocation(const std::string &address) {
//    return Location();
//}

//-------------------satic utils------------------------------------------------
bool Location::MarkDefined(const std::string &key, bool &flag,
                           const v_str &directive) {
    if (directive[0] == key) {
        if (directive.size() < 2) {
            ThrowLocationException(
                    "definition has to contain at least key and 1 "
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
            ThrowLocationException("Redefinition of " + key + " is not allowed");
        } else if (directive.size() < 2) {
            ThrowLocationException(
                    "definition has to contain at least key and 1 "
                    "value");
        } else {
            flag = true;
            return true;
        }
    }
    return false;
}

std::string Location::HandleAddressInConstructor(const std::string &address) {
    if (address == "/")
        return "";
    if (address.find_first_of('?') != std::string::npos)
        ThrowLocationException("? sign is not allowed in location address");
    if (address.find_first_of('/') != 0)
        ThrowLocationException("prefixed locations should be addressed with /");
    return SupressConsecutiveSlashes(address);
}

std::string Location::GetParticularAddress(const std::string &address) {
    if (address == "/")
        return address;
    std::string part_addr = SupressConsecutiveSlashes(address);
    return part_addr.substr(part_addr.find_last_of('/'));
}

std::string Location::SupressConsecutiveSlashes(const std::string &address) {
    std::string new_address = address;
    for (size_t i = 0; i < new_address.size(); ++i) {
        int consecutive_slashes = 0;
        while (new_address[i] == '/' && i < new_address.size()) {
            consecutive_slashes++;
            i++;
        }
        if (consecutive_slashes > 1) {
            new_address =
                    new_address.substr(0, i - consecutive_slashes + 1) +
                    new_address.substr(i);
        }
    }
    if (new_address != "/" && new_address[new_address.size() - 1] == '/')
        new_address = new_address.substr(0, new_address.size() - 1);
    return new_address;
}

v_str Location::SplitAddress(const std::string &address) {
    v_str result;
    if (address != "/") {
        std::string clean_address = SupressConsecutiveSlashes(address);
        std::string::size_type start = clean_address.find_last_of('/');
        while (!clean_address.empty()) {
            std::string step = clean_address.substr(start);
            result.push_back(step);
            clean_address = clean_address.substr(0, start);
            start = clean_address.find_last_of('/');
        }
        std::reverse(result.begin(), result.end());
    } else {
        result.push_back("/");
    }
    return result;
}

void Location::CheckSublocationsAddress(const std::string &address,
                                        const std::string &parent_address) {
    if (address.find_first_of('?') != std::string::npos)
        ThrowLocationException("? sign is not allowed in location address");
    if (address.find_first_of('/') != 0)
        ThrowLocationException("prefixed locations should be addressed with /");
    std::string full_address = SupressConsecutiveSlashes(address);
    std::string part_address = GetParticularAddress(full_address);
    if (!parent_address.empty() && (parent_address + part_address != full_address)) {
        ThrowLocationException("Full location address should start with parent "
                               "full address");
    }
}
//-------------------functional stuff-------------------------------------------
const Location & Location::GetMyRootRef() const {
    if (address_ == "/")
        return *this;
    else {
        l_loc_c_it ret = parent_;
        while (ret->address_ != "/")
            ret = ret->parent_;
        return *ret;
    }
}

l_loc_c_it Location::GetMyRootIt() const {
    l_loc_c_it ret = parent_;
    while (ret->address_ != "/")
        ret = ret->parent_;
    return ret;
}

// todo tests!!
const l_str &Location::GetIndeces() const {
    if (!has_own_index_defined_ && index_defined_in_parent_)
        return parent_->GetIndeces();
    return own_index_;
}

bool Location::HasDefinedLimitExcept() const {
    return !limit_except_.except_.empty();
}

s_err_c_it Location::FindErrPageForCode(int code) const {
    return error_pages_.find(ErrPage(code));
}

bool Location::HasErrPageForCode(int code) const {
    return FindErrPageForCode(code) != error_pages_.end();
}

l_loc_c_it Location::FindConstSublocationByAddress(const std::string &address) const {
    if (address == "/")
        return parent_;
    LocationByAddress to_find(address);
    l_loc_c_it it = std::find_if(sublocations_.begin(),
                                 sublocations_.end(),
                                 LocationByAddress(address));
    return it;
}

l_loc_it Location::FindSublocationByAddress(const std::string &address) {
    if (address == "/")
        return parent_;
    LocationByAddress to_find(address);
    l_loc_it it = std::find_if(sublocations_.begin(),
                                 sublocations_.end(),
                                 LocationByAddress(address));
    return it;
}

LocationByAddress::LocationByAddress(const std::string &targetAddress)
        : targetAddress_(targetAddress) {}

bool LocationByAddress::operator()(const Location &location) const {
    return location.address_ == targetAddress_;
}
//-------------------setup directives handlers----------------------------------
/**
 *  Updates some location parameters based on the directives
 *  @param directives
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
                ThrowLocationException("Error code is wrong");
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
        ThrowLocationException("Error page directive is wrong");
    }
}

/**
 * from nginx docs:
 *  Directives
 *  Syntax: 	index file ...;
 *  Default: 	index index.html;
 *  Context: 	http, server, location
 *
 *      Defines files that will be used as an index. The file name can contain
 *  variables. Files are checked in the specified order. The last element of
 *  the list can be a file with an absolute path.
 *  Example:
 *      index index.$geo.html index.0.html /index.html;
 *  It should be noted that using an index file causes an internal redirect,
 *  and the request can be processed in a different location. For example,
 *  with the following configuration:
 *
 *  location = / {
 *      index index.html;
 *  }
 *  location / {
 *      ...
 *  }
 *  a “/” request will actually be processed in the second location as
 *  “/index.html”.
 */
void Location::HandleIndex(const v_str &directives) {
    if (has_own_index_defined_) {
        for (size_t j = 1; j < directives.size(); ++j) {
            if (directives[j][0] == '/' && j != directives.size() - 1) {
                ThrowLocationException(
                        "Only the last element of the list can be a "
                        "file with an absolute path");
            } else {
                own_index_.push_back(directives[j]);
            }
        }
    } else {
        has_own_index_defined_ = true;
        for (size_t j = directives.size() - 1; j >= 1; --j) {
            if (directives[j][0] == '/' && j != directives.size() - 1) {
                ThrowLocationException(
                        "Only the last element of the list can be a "
                        "file with an absolute path");
            } else {
                own_index_.push_front(directives[j]);
            }
        }
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
        ThrowLocationException("Root directive is wrong");
    }
}
//-------------------setup subcontexts handlers---------------------------------
void Location::HandleLimitExcept(const Node &node) {
    if (HasDefinedLimitExcept()) {
        ThrowLocationException("Multiple definition of limit_except");
    } else {
        limit_except_.LimExHandleMethods(node.main_);
        limit_except_.LimExHandleDirectives(node.directives_);
    }
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
void Location::ThrowLocationException(const std::string &msg) {
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
    own_index_ = rhs.own_index_;
    has_own_index_defined_ = rhs.has_own_index_defined_;
    limit_except_ = rhs.limit_except_;
    return_code_ = rhs.return_code_;
    return_internal_address_ = rhs.return_internal_address_;
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
    if (return_internal_address_ != rhs.return_internal_address_)
        return false;
    if (root_ != rhs.root_)
        return false;
    if (address_ != rhs.address_)
        return false;
    if (sublocations_ != rhs.sublocations_)
        return false;
    return true;
}

void ptint_err_pages(std::ostream &os, const Location &location) {
    os << location.full_address_ << ":\t" << "Error Pages: " << std::endl;
    for (s_err_c_it it = location.error_pages_.begin();
         it != location.error_pages_.end(); ++it) {
        os << location.full_address_ <<  ":\t\t" << *it << std::endl;
    }
}

void print_return_info(std::ostream &os, const Location &location) {
    os << location.full_address_ << ":\t" << "Return Code: " <<
        location.return_code_ << std::endl;
    if (!location.return_internal_address_.empty()) {
        os << location.full_address_ << ":\t" << "Return Address: " <<
           location.return_internal_address_ << std::endl;
    } else if (!location.return_external_address_.empty()) {
        os << location.full_address_ << ":\t" << "Return Address: " <<
           location.return_external_address_ << std::endl;
    } else if (!location.return_custom_message_.empty()) {
        os << location.full_address_ << ":\t" << "Return Custom message: " <<
           location.return_custom_message_ << std::endl;
    }
}

void print_index(std::ostream &os, const Location &location) {
    os << location.full_address_ << ":\t" << "Index: ";
    for (l_str_c_it it = location.GetIndeces().begin();
         it != location.GetIndeces().end(); ++it) {
        os << *it << " ";
    }
    os << std::endl;
}

void print_root(std::ostream &os, const Location &location) {
    os << location.full_address_ << ":\t" << "Root: " <<
        location.root_ << std::endl;
}

void print_parent_info(std::ostream &os, const Location &location) {
    os << location.address_ << ":\t" << "Parent: " <<
        location.parent_->address_ << std::endl;
}

void print_sublocations(std::ostream &os, const Location &location) {
    for (l_loc_c_it it = location.sublocations_.begin();
         it != location.sublocations_.end(); ++it) {
        os << *it;
    }
}

std::ostream &operator<<(std::ostream &os, const Location &location) {
    os << std::endl << "Localion " << location.address_ << ":" <<
       std::endl;
    if (!location.error_pages_.empty())
        ptint_err_pages(os, location);
    if (location.return_code_ > 0)
        print_return_info(os, location);
    if (!location.GetIndeces().empty())
        print_index(os, location);
    if (!location.root_.empty())
        print_root(os, location);
    if (location.address_ != "/")
        print_parent_info(os, location);
    if (!location.sublocations_.empty())
        print_sublocations(os, location);
    return os;
}
