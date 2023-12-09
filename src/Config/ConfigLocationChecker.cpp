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

//bool Config::IsCorrectLimitExcept(Node &node, Location &current) {
//    if (IsLimitExcept(node)) {
//        if (LimExIsDefined(current))
//            ThrowSyntaxError("Limit_except context is already defined");
//        return true;
//    }
//    return false;
//}

//bool Config::IsCorrectLocation(const Node &node) {
//    if (IsLocation(node)) {
//        if (node.main_.size() != 2)
//            ThrowSyntaxError("Location path is incorrect or missing");
//        if (node.directives_.empty() && node.child_nodes_.empty())
//            ThrowSyntaxError("Location context can't be empty !");
//        return true;
//    }
//    return false;
//}

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