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

#include <cstdlib>
#include <algorithm>
#include "Config.h"

bool Config::IsLimitExcept(const Node &node) {
    if (node.main_[0] == "limit_except")
        return true;
    return false;
}

bool Config::IsCorrectLimitExcept(Node &node, Location &current) {
    if (IsLimitExcept(node)) {
        if (!IsCorrectLimit(node))
            ThrowSyntaxError("Limit_except context is incorrect");
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

bool Config::IsCorrectLimit(const Node &node) {
    if (node.main_.size() < 2 || node.directives_.empty())
        return false;
    Limit curr_limit;
    for (size_t i = 1; i < node.main_.size(); ++i) {
        if (node.main_[i] != "GET" && node.main_[i] != "POST" &&
            node.main_[i] != "DELETE") {
            return false;
        }
    }
//    TODO what else can be in limit_except?
    for (size_t i = 0; i < node.directives_.size(); ++i) {
        if (!(node.directives_[i][0] == "deny" &&
            node.directives_[i][1] == "all")) {
            return false;
        }
    }
    return true;
//    ThrowSyntaxError("Given HTTP method isn't supported or doesn't exist");
//    ThrowSyntaxError("HTTP methods needs to be specified");
//    ThrowSyntaxError("Limit_except context can't be empty !");
}

//todo check is redefinition of limit_except is allowed
void
Config::HandleLimitExceptContext(Node &node, Limit &curr_limit) const {
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
            ThrowSyntaxError("Seems like there are repeatable methods in the "
                             "limit_except block");
        }
    }
    if (node.directives_.size() == 1 || node.directives_.size() == 2) {}
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

//            if (node.directives_[0][0] == "deny" &&
//                node.directives_[0][1] == "all" &&
//                node.directives_[0].size() == 2 ) {
//                curr_limit.return_code_ = 403;
//            } else if (node.directives_[0][0] == "return" &&
//                        (node.directives_[0].size() == 2 ||
//                        node.directives_[0].size() == 3)) {
//                curr_limit.return_code_ = std::atoi(node.directives_[0][1].c_str());
//                if (Location::kHttpOkCodes.find(curr_limit.return_code_) ==
//                    Location::kHttpOkCodes.end() &&
//                    ErrPage::kHttpErrCodes.find(curr_limit.return_code_) ==
//                    ErrPage::kHttpErrCodes.end()) {
//                    ThrowSyntaxError("return directive is wrong");
//                }
//                if (node.directives_[0].size() == 3) {
//                    if (node.directives_[0][2].find_first_of('/') !=
//                        std::string::npos) {
//                        curr_limit.return_address_ = node.directives_[0][2];
//                    } else {
//                        ThrowSyntaxError("Limit_except context needs 1 of these "
//                                         "directives: return or deny !");
//                    }
//                }
//            }