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

//const static int kRoot = 0;
//const static int kReturn = 1;
//const static int kIndex = 2;
//const static int kLimit = 3;
//const static int kDirectivesAmount = 4;


void Config::AddErrorPages(const v_strings &directive, Location &location) const {
    std::string address;
    std::vector<int> code;

    for (size_t j = 1; j < directive.size() &&
                       IsNumber(directive[j]); ++j) {
        code.push_back(std::atoi(directive[j].c_str()));
    }
    address = (*(directive.rbegin()));
    location.error_pages_.push_back(ErrPage(address, code));
}

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

void Config::CheckLocationDirectives(const Node &loc_node, Location &current_l,
                                     bool &set_root, bool &set_index, bool &ret,
                                     bool &err_pages) const {
    for (size_t i = 0; i < loc_node.directives_.size(); ++i) {
        if (UMarkDefined("root", set_root, loc_node.directives_[i]))
            current_l.root_ = loc_node.directives_[i][1];
        if (MarkDefined("index", set_index, loc_node.directives_[i])) {
            for (size_t j = 1; j < loc_node.directives_[i].size(); ++j) {
                current_l.index_.push_back(loc_node.directives_[i][j]);
            }
        }
        if (UMarkDefined("return", ret, loc_node.directives_[i])) {
            HandleLocationReturn(loc_node, current_l, i);
        }
        if (MarkDefined("error_page", err_pages, loc_node.directives_[i])) {
            AddErrorPages(loc_node.directives_[i], current_l);
        }
    }
}

void Config::CheckLimitExceptContext(ConfigNode &node, Location &location,
                                     bool &limit) const {
    if (node.main_[0] == "limit_except") {
        limit = true;
        bool deny = false;
        bool ret = false;

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
            if (UMarkDefined("deny", deny, node.directives_[i])) {
                location.limit_except_action_ = DENY;
            } else if (UMarkDefined("return", ret, node.directives_[i])) {
                location.limit_except_return_code_ =
                        std::atoi(node.directives_[i][1].c_str());
            }
        }
//        for (size_t i = 0; i < node.child_nodes_.size(); ++i) {
//            CheckIfCondition(node.child_nodes_[i], set_ret);
//        }TODO probably too much
        if (!ret && !deny)
            ThrowSyntaxError("Limit_except context needs at least 1 of these "
                             "directives (directly or in subcontext): return "
                             "or deny !");
    }
}

void Config::CheckLocation(Node &loc_node, Location &current_l) {
    if (loc_node.main_[0] == "location") {
        bool        ret = false, limit = false, root = false,
                    index = false, err = false;

        if (loc_node.main_.size() != 2)
            ThrowSyntaxError("Location path is incorrect or missing");
        if (loc_node.directives_.empty())
            ThrowSyntaxError("Location context can't be empty !");
        current_l.address_ = loc_node.main_[1];
        for (size_t i = 0; i < loc_node.child_nodes_.size(); ++i) {
            CheckLimitExceptContext(loc_node.child_nodes_[i],
                                    current_l, limit);
        }
        CheckLocationDirectives(loc_node, current_l, root, index, ret,
                                err);
        if (!index && !ret && !limit && !root && !err)
            ThrowSyntaxError("Location context should contain at least one of "
                             "following directives (directly or in "
                             "subcontext): root, index, or return!");
    }
}

void Config::HandleLocationContext(Node &maybe_loc_context,
                                   ServerConfiguration &current_srv) {
    if (maybe_loc_context.main_[0] == "location") {
        std::string &address = maybe_loc_context.main_[1];
        if (address == "/" && !current_srv.explicit_default_location_set_) {
            current_srv.explicit_default_location_set_ = true;
            CheckLocation(maybe_loc_context,
                          current_srv.locations_[0]);
        } else {
            for (size_t i = 0; i < current_srv.locations_.size(); ++i) {
                if (current_srv.locations_[i].address_ == address) {
                    ThrowSyntaxError("Multiple locations for the same address "
                                     "inside one server context");
                }
            }
            Location    current_l;
            CheckLocation(maybe_loc_context, current_l);
            current_srv.locations_.push_back(current_l);
        }
    }
}

