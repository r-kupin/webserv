/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigChecker.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/10 20:18:56 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include "Config.h"

const static int kListen = 0;
const static int kName = 1;
const static int kRoot = 2;
const static int kIndex = 3;
const static int kErrorPage = 4;
const static int kEssentialDirectivesAmount = 5;

void MarkDefined(const std::string& key, const v_strings &directive,
                 bool &flag) {
    if (directive[0] == key && directive.size() > 1)
        flag = true;
}

void Config::CheckServerDirectives(Config::Node &node,
                                   bool *set) const {
    for (size_t i = 0; i < node.directives_.size(); i++) {
        MarkDefined("server_name", node.directives_[i], set[kName]);
        MarkDefined("listen", node.directives_[i], set[kListen]);
        MarkDefined("root", node.directives_[i], set[kRoot]);
        MarkDefined("index", node.directives_[i], set[kIndex]);
        MarkDefined("error_page", node.directives_[i], set[kErrorPage]);
    }
}

void
Config::CheckLimitExceptContext(Config::ConfigNode &node, bool &set_ret) const {
    if (node.main_[0] == "limit_except") {
        bool deny = false;

        if (node.main_.size() < 2)
            ThrowSyntaxError("HTTP methods needs to be specified");
        if (!node.directives_.empty()) {
            for (size_t i = 1; i < node.main_.size(); ++i) {
                if (node.main_[i] != "GET" && node.main_[i] != "POST" &&
                    node.main_[i] != "DELETE") {
                    ThrowSyntaxError("Given HTTP method isn't supported or"
                                     " doesn't exist");
                }
            }
        }
        for (size_t i = 0; i < node.directives_.size(); ++i) {
            MarkDefined("deny", node.directives_[i], deny);
//            MarkDefined("return", node.directives_[i], set_ret);
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

void Config::CheckLocationContext(Config::Node &node, bool &set_root,
                                  bool &set_index) const {
    if (node.main_[0] == "location") {
        bool ret;

        if (node.main_.size() != 2)
            ThrowSyntaxError("Location path is incorrect or missing");
        if (node.directives_.empty())
            ThrowSyntaxError("Location context can't be empty !");
        for (size_t i = 0; i < node.directives_.size(); ++i) {
            MarkDefined("root", node.directives_[i], set_root);
            MarkDefined("index", node.directives_[i], set_index);
            MarkDefined("return", node.directives_[i], ret);
        }
        for (size_t i = 0; i < node.child_nodes_.size(); ++i) {
            CheckLimitExceptContext(node.child_nodes_[i], ret);
        }
        if (!set_root && !set_index && !ret)
            ThrowSyntaxError("Location context should contain at least one of"
                             " following directives (directly or in "
                             "subcontext): root, index, or return!");
    }
}

void Config::CheckServer(Node &node) {
    bool set[kEssentialDirectivesAmount] = {false};

    for (size_t i = 0; i < node.child_nodes_.size(); i++) {
        CheckLocationContext(node.child_nodes_[i], set[kRoot],
                             set[kIndex]);
    }
    CheckServerDirectives(node, set);
    for (int i = 0; i < kEssentialDirectivesAmount; ++i) {
        if (!set[i])
            ThrowSyntaxError("Server context is lacking of some essential "
                             "directive(s)!");
    }
}


void Config::CheckComponents(Node& root) {
    if (!root.directives_.empty()) {
        std::cout << "Found directive(s) inside main context" << std::endl;
        std::cout << "Only \"server\" blocks are allowed inside a main "
                     "context, everything else will be ignored" << std::endl;
    }
    for (size_t i = 0; i < root.child_nodes_.size(); i++) {
        if (root.child_nodes_[i].main_[0] == "server") {
            CheckServer(root.child_nodes_[i]);
            servers_++;
        } else {
            std::cout << "Found block " + root.child_nodes_[i].main_[0] +
                    " inside main context" << std::endl;
            std::cout << "Only \"server\" blocks are allowed inside a main "
                         "context, everything else will be ignored" <<
                         std::endl;
        }
    }
    if (servers_ == 0) {
        ThrowSyntaxError("At least one server needs to be defined in the "
                         "main context of a config file");
    }
}
