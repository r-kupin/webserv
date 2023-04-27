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
#include <string>
#include <cstdlib>
#include "Config.h"

const static int kListen = 0;
const static int kName = 1;
const static int kRoot = 2;
const static int kIndex = 3;
const static int kErrorPage = 4;
const static int kEssentialDirectivesAmount = 5;

void Config::CheckServerDirectives(Node &node, bool *set,
                                   ServerConfiguration &current) const {
    for (size_t i = 0; i < node.directives_.size(); i++) {
        if (MarkDefined("server_name", set[kName], node.directives_[i])) {
            for (size_t j = 1; j < node.directives_[i].size(); ++j) {
                current.server_name_.push_back(node.directives_[i][j]);
            }
        } else if (MarkDefined("listen", set[kListen], node.directives_[i])) {
            current.port_ = atoi(node.directives_[i][1].c_str());
        } else if (MarkDefined("root", set[kRoot], node.directives_[i])) {
            current.root_ = node.directives_[i][1];
        } else if (MarkDefined("index", set[kIndex], node.directives_[i])) {
            for (size_t j = 1; j < node.directives_[i].size(); ++j) {
                current.index_.push_back(node.directives_[i][j]);
            }
        } else if (MarkDefined("error_page",
                               set[kErrorPage], node.directives_[i])) {
            ErrPage errPage;
            for (size_t j = 1; j < node.directives_[i].size() &&
                    IsNumber(node.directives_[i][j]); ++j) {
                errPage.code_.push_back(std::atoi(node.directives_[i][j].c_str()));
            }
            errPage.address_ = (*(node.directives_[i].rbegin()));
            current.error_pages_.push_back(errPage);
        }
    }
}

/**
 * Checks server node, and creates a server config based on the content of node
 * 1. Making a set to keep track of the crucial parameters of the server. If
 *    we encounter the definition of a parameter - mark corresponding bool flag
 *    as true
 * 2. Cycle through the subnodes - and add found params to server config
 * 3. Check directives - and add found params to server config
 * 4. Check that all crucial parameters are defined.
 * @throw SyntaxError if something is missing
 * @param node of the server block we are currently checking
 * @return ready-to-use server configuration
 */
ServerConfiguration Config::CheckServer(Node &node) {
    bool set[kEssentialDirectivesAmount] = {false};
    ServerConfiguration current;

    for (size_t i = 0; i < node.child_nodes_.size(); i++) {
        HandleLocationContext(node.child_nodes_[i], set[kRoot],
                              set[kIndex], current);
    }
    CheckServerDirectives(node, set, current);
    for (int i = 0; i < kEssentialDirectivesAmount; ++i) {
        if (!set[i])
            ThrowSyntaxError("Server context is lacking of some essential "
                             "directive(s)!");
    }
    return current;
}

void Config::HandleServerContext(ConfigNode &srv_node,
                                 std::vector<ServerConfiguration> &servers) {
    int port = -1;
    for (size_t i = 0; i < srv_node.directives_.size(); ++i) {
         if (srv_node.directives_[i][0] == "listen") {
             port = atoi(srv_node.directives_[i][1].c_str());
         }
    }
    if (port != -1) {
        for (size_t i = 0; i < servers.size(); ++i) {
            if (servers[i].port_ == port)
                ThrowSyntaxError("Found multiple servers with the same port");
        }
    }
    servers.push_back(CheckServer(srv_node));
}

/**
 * Checks directives and blocks of a main context, creating server
 * configurations at the same time
 * @param root node of a parsed config
 */
std::vector<ServerConfiguration> Config::CheckComponents(Node& root) {
    if (!root.directives_.empty()) {
        std::cout << "Found directive(s) inside main context" << std::endl;
        std::cout << "Only \"server\" blocks are allowed inside a main "
                     "context, everything else will be ignored" << std::endl;
    }
    std::vector<ServerConfiguration> servers;
    for (size_t i = 0; i < root.child_nodes_.size(); i++) {
        if (root.child_nodes_[i].main_[0] == "server") {
            HandleServerContext(root.child_nodes_[i], servers);
        } else {
            std::cout << "Found block " + root.child_nodes_[i].main_[0] +
                    " inside main context" << std::endl;
            std::cout << "Only \"server\" blocks are allowed inside a main "
                         "context, everything else will be ignored" <<
                         std::endl;
        }
    }
    if (servers.empty()) {
        ThrowSyntaxError("At least one server needs to be defined in the "
                         "main context of a config file");
    }
    return servers;
}
