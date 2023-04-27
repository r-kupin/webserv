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
#include "Config.h"

void Config::HandleLocationReturn(const Node &node,
                                 Location &current_l,
                                  size_t i) const {
    for (size_t j = 1; j < node.directives_[i].size(); ++j) {
        if (!current_l.return_code_ && IsNumber(node.directives_[i][j])) {
            current_l.return_code_ = atoi(node.directives_[i][j].c_str());
        } else if (current_l.return_address_.empty() &&
                node.directives_[i][j].find_first_of('/') !=
                                                            std::string::npos) {
            current_l.return_address_ = node.directives_[i][j];
        } else
            ThrowSyntaxError("only HTTP code and / or address can follow "
                          "\"return\" keyword, and none can occur repeatedly");

    }
}

void Config::CheckLocationDirectives(const Node &loc_node,
                                     Location &current_l,
                                     bool &set_root, bool &set_index,
                                     bool &ret) const {
    for (size_t i = 0; i < loc_node.directives_.size(); ++i) {
        if (MarkDefined("root", set_root, loc_node.directives_[i]))
            current_l.root_ = loc_node.directives_[i][1];
        if (MarkDefined("index", set_index, loc_node.directives_[i])) {
            for (size_t j = 1; j < loc_node.directives_[i].size(); ++j) {
                current_l.index_.push_back(loc_node.directives_[i][j]);
            }
        }
        if (MarkDefined("return", ret, loc_node.directives_[i])) {
            HandleLocationReturn(loc_node, current_l, i);
        }
    }
}

void Config::CheckLimitExceptContext(ConfigNode &node, bool &set_ret,
                                     Location &location, bool &limit) const {
    if (node.main_[0] == "limit_except") {
        limit = true;
        bool deny = false;

        if (node.main_.size() < 2)
            ThrowSyntaxError("HTTP methods needs to be specified");
        if (node.directives_.empty())
            ThrowSyntaxError("Limit_except context can't be empty !");
        for (size_t i = 1; i < node.main_.size(); ++i) {
            if (node.main_[i] == "GET") {
                location.limit_except_methods_.insert(GET);
            } else if (node.main_[i] == "POST") {
                location.limit_except_methods_.insert(POST);
            } else if (node.main_[i] == "DELETE") {
                location.limit_except_methods_.insert(DELETE);
            } else {
                ThrowSyntaxError("Given HTTP method isn't supported or"
                                 " doesn't exist");
            }
        }
        for (size_t i = 0; i < node.directives_.size(); ++i) {
            if (MarkDefined("deny", deny, node.directives_[i])) {
                location.limit_except_action_ = DENY;
            } else if (MarkDefined("return", set_ret, node.directives_[i])) {
                location.limit_except_return_code_ =
                        std::atoi(node.directives_[i][1].c_str());
            }
        }
//        for (size_t i = 0; i < node.child_nodes_.size(); ++i) {
//            CheckIfCondition(node.child_nodes_[i], set_ret);
//        }TODO probably too much
        if (!set_ret && !deny)
            ThrowSyntaxError("Limit_except context needs at least 1 of these "
                             "directives (directly or in subcontext): return "
                             "or deny !");
    }
}

void Config::CheckLocation(Node &loc_node, bool &set_root,
                           bool &set_index,
                           ServerConfiguration &current_s) {
    if (loc_node.main_[0] == "location") {
        Location current_l;
        bool ret = false;
        bool limit = false;
        bool root = false;

        if (loc_node.main_.size() != 2)
            ThrowSyntaxError("Location path is incorrect or missing");
        if (loc_node.directives_.empty())
            ThrowSyntaxError("Location context can't be empty !");
        current_l.address_ = loc_node.main_[1];
        for (size_t i = 0; i < loc_node.child_nodes_.size(); ++i) {
            CheckLimitExceptContext(loc_node.child_nodes_[i], ret,
                                    current_l, limit);
        }
        CheckLocationDirectives(loc_node, current_l, root, set_index, ret);
        if (root)
            set_root = true;
        if (!set_index && !ret && !limit && !root)
            ThrowSyntaxError("Location context should contain at least one of "
                             "following directives (directly or in "
                             "subcontext): root, index, or return!");
        current_s.locations_.push_back(current_l);
    }
}

void Config::HandleLocationContext(Node &maybe_loc_context, bool &set_root,
                                   bool &set_index,
                                   ServerConfiguration &current_srv) {
    if (maybe_loc_context.main_[0] == "location") {
        std::string &address = maybe_loc_context.main_[1];
        for (size_t i = 0; i < current_srv.locations_.size(); ++i) {
            if (current_srv.locations_[i].address_ == address) {
                ThrowSyntaxError("Multiple locations for the same address "
                                 "inside one server context");
            }
        }
        CheckLocation(maybe_loc_context, set_root, set_index, current_srv);
    }
}
