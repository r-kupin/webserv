#include <algorithm>
#include <iostream>
#include "LimitExcept.h"


Limit::Limit() : deny_all_(false), allow_all_(false) {}

bool is_deny_all(const v_str &directives) {
    return directives.size() == 2 &&
           directives[0] == "deny" &&
           directives[1] == "all";
}

bool is_allow_all(const v_str &directives) {
    return directives.size() == 2 &&
           directives[0] == "allow" &&
           directives[1] == "all";
}

bool is_deny_address(const v_str &directives) {
    return directives.size() > 1 &&
           directives[0] == "deny";
}

bool is_allow_address(const v_str &directives) {
    return directives.size() > 1 &&
           directives[0] == "allow";
}


void Limit::LimExHandleDirectives(const std::vector<v_str> &directives) {
    if (directives.empty())
        ThrowLimitExceptError("At least one of these directives should be  "
                              "specified in limit_except context: deny: all "
                              "or address or allow: all or address");
    for (vstr_vstr_c_it it = directives.begin(); it != directives.end(); ++it) {
        if (it->size() < 2)
            ThrowLimitExceptError("Limit_except context needs 1 of these "
                                  "directives: deny: all or address or allow:"
                                  " all  or address");
        if (is_deny_all(*it)) {
            deny_all_ = true;
        } else if (is_allow_all(*it)) {
            allow_all_ = true;
        } else if (is_deny_address(*it)) {
            for (v_str_c_it it_it = it->begin() + 1; it_it != it->end(); ++it_it)
                LimExDenyAddress(*it_it);
        } else if (is_allow_address(*it)) {
            for (v_str_c_it it_it = it->begin() + 1; it_it != it->end(); ++it_it)
                LimExAllowAddress(*it_it);
        } else {
            ThrowLimitExceptError("Limit_except context needs deny: all or  "
                                  "address  or allow: all or address");
        }
    }
    if (deny_all_ && allow_all_)
        ThrowLimitExceptError("deny: all and allow: all can't be specified "
                           "simultaneously");
}

void Limit::LimExHandleMethods(const v_str &main) {
    if (main.size() < 2)
        ThrowLimitExceptError("Limit_except context needs at least one HTTP "
                              "method");
    for (v_str_c_it it = main.begin() + 1; it != main.end(); ++it) {
        if (*it == "GET" && except_.find(GET) == except_.end()) {
            except_.insert(GET);
        } else if (*it == "POST" && except_.find(POST) == except_.end()) {
            except_.insert(POST);
        } else if (*it == "DELETE" && except_.find(DELETE) == except_.end()) {
            except_.insert(DELETE);
        } else {
            ThrowLimitExceptError("Seems like there are repeatable or  "
                                  "unsupported  methods methods, in the "
                                  "limit_except block");
        }
    }
    if (except_.empty())
        ThrowLimitExceptError("Limit_except context needs at least one HTTP "
                              "method");
}

void Limit::LimExDenyAddress(const std::string &address) {
    if (!allow_.empty() &&
        std::find(allow_.begin(), allow_.end(),
                  address) != allow_.end()) {
        ThrowLimitExceptError("can't deny and allow the same address");
    }
    deny_.push_back(address);
}

void Limit::LimExAllowAddress(const std::string &address) {
    if (!deny_.empty() &&
        std::find(deny_.begin(),deny_.end(),
                  address) != deny_.end()) {
        ThrowLimitExceptError("can't deny and allow the same address");
    }
    allow_.push_back(address);
}

void Limit::ThrowLimitExceptError(const std::string &msg) const {
    std::cout << "Limit except syntax error: " + msg << std::endl;
    throw LimitExceptException();
}

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
