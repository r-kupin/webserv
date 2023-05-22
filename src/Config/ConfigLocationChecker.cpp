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

bool Config::IsLocation(const Node &node) {
    if (node.main_[0] == "location")
        return true;
    return false;
}

bool Config::IsLimitExcept(const Node &node) {
    if (node.main_[0] == "limit_except")
        return true;
    return false;
}

bool Config::IsCorrectLocation(const Node &node) {
    if (node.main_.size() == 2 &&
        (!node.directives_.empty() || !node.child_nodes_.empty()))
        return true;
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
    return true;
//    ThrowSyntaxError("Given HTTP method isn't supported or doesn't exist");
//    ThrowSyntaxError("HTTP methods needs to be specified");
//    ThrowSyntaxError("Limit_except context can't be empty !");
}

void Config::UpdateIndex(const v_strings &directive, Location &location) {
    if (!location.default_index_) {
        location.index_.clear();
        location.default_index_ = true;
    }
    for (size_t i = 1; i < directive.size(); ++i) {
        location.index_.insert(directive[i]);
    }
}

void Config::AddErrorPages(const v_strings &directive, Location &location) {
    std::set<ErrPage>   &all_err_pages = location.error_pages_;
    std::string         address;
    int                 code;
    
    for (size_t j = 1; j < directive.size() && IsNumber(directive[j]); ++j) {
        address = *(directive.rbegin());
        code = std::atoi(directive[j].c_str());
        ErrPage err_page(address, code);
        const std::set<ErrPage>::iterator & iterator =
                                                all_err_pages.find(err_page);
        if (iterator == all_err_pages.end()) {
            all_err_pages.insert(err_page);
        } else {
            ErrPage &old = const_cast<ErrPage &>(*iterator);
            old.address_ = address;
        }
    }
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

void
Config::HandleLimitExceptContext(ConfigNode &node, Limit &curr_limit) const {
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
    if (node.directives_.size() == 1) {
        if (node.directives_[0][0] == "deny" &&
            node.directives_[0][1] == "all") {
            curr_limit.return_code_ = 403;
        } else if (node.directives_[0][0] == "return") {
            curr_limit.return_code_ = std::atoi(
                                        node.directives_[0][1].c_str());
        }
    }
    if (curr_limit.return_code_ == -1) {
        ThrowSyntaxError("Limit_except context needs 1 of these "
                         "directives: return or deny !");
    }
//        for (size_t i = 0; i < node.directives_.size(); ++i) {
//            if (UMarkDefined("deny", deny, node.directives_[i])) {
//                curr_limit.deny_all_ = true;
//            } else if (UMarkDefined("return", ret, node.directives_[i])) {
//                curr_limit.return_code_ = std::atoi(
//                                            node.directives_[i][1].c_str());
//            }
//        }
//        for (size_t i = 0; i < node.child_nodes_.size(); ++i) {
//            CheckIfCondition(node.child_nodes_[i], set_ret);
//        }TODO probably too much
}
//
//void Config::CheckLocation(Node &loc_node, Location &current_l) {
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
//        CheckLocationDirectives(loc_node, current_l, root, index, ret,
//                                err);
//        if (!index && !ret && !limit && !root && !err)
//            ThrowSyntaxError("Location context should contain at least one of "
//                             "following directives (directly or in "
//                             "subcontext): root, index, or return!");
//    }
//}

void Config::CheckLocationDirectives(Node &loc_node, Location &current_l) {
    bool    root = false,
            index = false,
            ret = false,
            err = false;

    for (size_t i = 0; i < loc_node.directives_.size(); ++i) {
        if (UMarkDefined("root", root, loc_node.directives_[i]))
            current_l.root_ = loc_node.directives_[i][1];
        if (MarkDefined("index", index, loc_node.directives_[i])) {
            UpdateIndex(loc_node.directives_[i], current_l);
        }
        if (UMarkDefined("return", ret, loc_node.directives_[i])) {
            HandleLocationReturn(loc_node, current_l, i);
        }
        if (MarkDefined("error_page", err, loc_node.directives_[i])) {
            AddErrorPages(loc_node.directives_[i], current_l);
        }
    }
}

void Config::HandleLocationContext(Node &loc_context, Location &parent) {
    Location    maybe_current(loc_context.main_[1]);
    CheckSieblingsAdresses(parent, maybe_current);
    Location    &current = (maybe_current.HasSameAddress(parent)) ?
                                                        parent : maybe_current;

    CheckLocationDirectives(loc_context, current);
    for (std::vector<ConfigNode>::iterator it = loc_context.child_nodes_.begin();
         it != loc_context.child_nodes_.end(); ++it) {
        if (IsLocation(*it)) {
            if (!IsCorrectLocation(*it))
                ThrowSyntaxError("Location is incorrect");
            HandleLocationContext(*it, current);
        } else if (IsLimitExcept(*it)) {
            if (!IsCorrectLimit(*it))
                ThrowSyntaxError("Limit except is incorrect");
            HandleLimitExceptContext(*it, current.limit_except_);
        }
    }
    if (!(current.HasSameAddress(parent)))
        parent.sublocations_.insert(current);
//    for (size_t i = 0; i < loc_context.child_nodes_.size(); ++i) {
//        if (parent == current && !overriden_parrent_) {
//            overriden_parrent_ = true;
//            sublocation = true;
//            CheckLocation(loc_context, parent);
//        } else if (parent.sublocations_.find(current) ==
//                    parent.sublocations_.end()) {
//            CheckLocation(loc_context, current);
//            parent.sublocations_.insert(current);
//        } else {
//            ThrowSyntaxError("Multiple locations for the same address "
//                             "inside one server context");
//        }
//    }
}

void Config::CheckSieblingsAdresses(const Location &parent,
                                    const Location &maybe_current) const {
    for (std::_Rb_tree_const_iterator<Location> it = parent.sublocations_.begin();
         it != parent.sublocations_.end(); ++it) {
        if (it->HasSameAddress(maybe_current))
            ThrowSyntaxError(
                    "Each location needs unique address inside each context");
    }
}

