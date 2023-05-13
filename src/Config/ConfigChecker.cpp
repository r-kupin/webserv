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
#include <algorithm>
#include <cstdlib>
#include "Config.h"

void Config::CheckServerDirectives(Node &node, bool &port,
                                   ServerConfiguration &current) const {
    bool srv_name = false;
    bool cl_max_bd_size = false;
    bool err = false;
    bool index = false;
    bool root = false;

    for (size_t i = 0; i < node.directives_.size(); i++) {
        if (MarkDefined("server_name", srv_name, node.directives_[i])) {
            // TODO server names - hostnames ....
            for (size_t j = 1; j < node.directives_[i].size(); ++j)
                current.server_names_.push_back(node.directives_[i][j]);
        } else if (UMarkDefined("listen", port, node.directives_[i])) {
            current.port_ = atoi(node.directives_[i][1].c_str());
            current.port_str_ = node.directives_[i][1];
        } else if (UMarkDefined("client_max_body_size", cl_max_bd_size,
                                node.directives_[i])) {
            current.client_max_body_size_ = atoi(node.directives_[i][1].c_str());
        } else if (UMarkDefined("root", root, node.directives_[i])) {
            current.locations_[0].root_ = node.directives_[i][1];
        } else if (MarkDefined("index", index, node.directives_[i])) {
            for (size_t j = 1; j < node.directives_[i].size(); ++j) {
                current.locations_[0].index_.push_back(node.directives_[i][j]);
            }
        } else if (MarkDefined("error_page", err, node.directives_[i])) {
            AddErrorPages(node.directives_[i], current.locations_[0]);
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
void
Config::CheckServer(Node &node, std::vector<ServerConfiguration> &servers) {
    ServerConfiguration current;
    bool port = false;

    CheckServerDirectives(node, port, current);
    for (size_t i = 0; i < node.child_nodes_.size(); i++) {
        HandleLocationContext(node.child_nodes_[i], current);
//        CheckLimitExceptContext();
    }
    if (!port)
        ThrowSyntaxError("Port needs to be specified explicitly!");
    servers.push_back(current);
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
            CheckServer(root.child_nodes_[i], servers);
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
