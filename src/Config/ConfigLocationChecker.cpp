/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigLocationChecker.cpp                          :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/18 22:08:40 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <algorithm>
#include "Config.h"

bool Config::IsLimitExcept(const Node &node) {
    if (node.main_[0] == "limit_except")
        return true;
    return false;
}

bool Config::IsCorrectLimitExcept(Node &node, Location &current) {
    if (IsLimitExcept(node)) {
        if (LimExIsDefined(current))
            ThrowSyntaxError("Limit_except context is already defined");
        return true;
    }
    return false;
}

bool Config::IsLocation(const Node &node) {
    if (node.main_[0] == "location")
        return true;
    return false;
}

bool Config::IsCorrectLocation(const Node &node) {
    if (IsLocation(node)) {
        if (node.main_.size() != 2)
            ThrowSyntaxError("Location path is incorrect or missing");
        if (node.directives_.empty() && node.child_nodes_.empty())
            ThrowSyntaxError("Location context can't be empty !");
        return true;
    }
    return false;
}

void
Config::HandleLimitExceptContext(Node &node, Limit &curr_limit) const {
    CheckHTTPMethodsLimitExcept(node, curr_limit);
    CheckDirectivesLimitExcept(node, curr_limit);
}

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

void
Config::CheckDirectivesLimitExcept(const Node &node, Limit &curr_limit) const {
    if (node.directives_.empty())
        ThrowSyntaxError("At least one of these directives should be "
                         "specified in limit_except context: deny: all or "
                         "address or allow: all or address");
    for (size_t i = 0; i < node.directives_.size(); ++i) {
        if (node.directives_[i].size() == 1)
            ThrowSyntaxError("Limit_except context needs 1 of these "
                             "directives: deny: all or address or allow: all "
                             "or address");
        if (is_deny_all(node.directives_[i])) {
            curr_limit.deny_all_ = true;
        } else if (is_allow_all(node.directives_[i])) {
            curr_limit.allow_all_ = true;
        } else if (is_deny_address(node.directives_[i])) {
            for (size_t j = 1; j < node.directives_[i].size(); ++j)
                deny_address(node.directives_[i][j], curr_limit);
        } else if (is_allow_address(node.directives_[i])) {
            for (size_t j = 1; j < node.directives_[i].size(); ++j)
                allow_address(node.directives_[i][j], curr_limit);
        } else {
            ThrowSyntaxError("Limit_except context needs deny: all or address "
                             "or allow: all or address");
        }
    }
    if (curr_limit.deny_all_ && curr_limit.allow_all_)
        ThrowSyntaxError("deny: all and allow: all can't be specified "
                         "simultaneously");
}

void Config::deny_address(const std::string &address, Limit &curr_limit) const {
    if (!curr_limit.allow_.empty() &&
        std::find(curr_limit.allow_.begin(), curr_limit.allow_.end(),
                  address) != curr_limit.allow_.end()) {
        ThrowSyntaxError("can't deny and allow the same address");
    }
    curr_limit.deny_.push_back(address);
}

void Config::allow_address(const std::string &address, Limit &curr_limit)const {
    if (!curr_limit.deny_.empty() &&
        std::find(curr_limit.deny_.begin(),curr_limit.deny_.end(),
                  address) != curr_limit.deny_.end()) {
        ThrowSyntaxError("can't deny and allow the same address");
    }
    curr_limit.allow_.push_back(address);
}

void Config::CheckHTTPMethodsLimitExcept(Node &node, Limit &curr_limit) const {
    for (std::vector<std::string>::iterator it = node.main_.begin() + 1;
         it != node.main_.end(); ++it) {
        if (*it == "GET" &&
            curr_limit.except_.find(GET) == curr_limit.except_.end()) {
            curr_limit.except_.insert(GET);
        } else if (*it == "POST" &&
            curr_limit.except_.find(POST) == curr_limit.except_.end()) {
            curr_limit.except_.insert(POST);
        } else if (*it == "DELETE" &&
            curr_limit.except_.find(DELETE) == curr_limit.except_.end()) {
            curr_limit.except_.insert(DELETE);
        } else {
            ThrowSyntaxError("Seems like there are repeatable or unsupported "
                             "methods methods, in the limit_except block");
        }
    }
    if (curr_limit.except_.empty())
        ThrowSyntaxError("Limit_except context needs at least one HTTP method");
}

const std::list<ServerConfiguration> &Config::getServers() const {
    return servers_;
}


//
//void Config::IsCorrectLocation(Node &loc_node, Location &current_l) {
//    if (loc_node.main_[0] == "location") {
//        bool        ret = false, limit = false, root = false,
//                    index = false, err = false;
//
//        if (loc_node.main_.size() != 2)
//            ThrowSyntaxError("Location path is incorrect or missing");
//        current_l.address_ = loc_node.main_[1];
//        for (size_t i = 0; i < loc_node.child_nodes_.size(); ++i) {
//            HandleLimitExceptContext(loc_node.child_nodes_[i],
//                                     current_l);
//        }
//        ProcessLocationDirectives(loc_node, current_l, root, index, ret,
//                                err);
//        if (!index && !ret && !limit && !root && !err)
//            ThrowSyntaxError("Location context should contain at least one of "
//                             "following directives (directly or in "
//                             "subcontext): root, index, or return!");
//    }
//}

//            if (node_.directives_[0][0] == "deny" &&
//                node_.directives_[0][1] == "all" &&
//                node_.directives_[0].size() == 2 ) {
//                curr_limit.return_code_ = 403;
//            } else if (node_.directives_[0][0] == "return" &&
//                        (node_.directives_[0].size() == 2 ||
//                        node_.directives_[0].size() == 3)) {
//                curr_limit.return_code_ = std::atoi(node_.directives_[0][1].c_str());
//                if (Location::kHttpOkCodes.find(curr_limit.return_code_) ==
//                    Location::kHttpOkCodes.end() &&
//                    ErrPage::kHttpErrCodes.find(curr_limit.return_code_) ==
//                    ErrPage::kHttpErrCodes.end()) {
//                    ThrowSyntaxError("return directive is wrong");
//                }
//                if (node_.directives_[0].size() == 3) {
//                    if (node_.directives_[0][2].find_first_of('/') !=
//                        std::string::npos) {
//                        curr_limit.return_address_ = node_.directives_[0][2];
//                    } else {
//                        ThrowSyntaxError("Limit_except context needs 1 of these "
//                                         "directives: return or deny !");
//                    }
//                }
//            }