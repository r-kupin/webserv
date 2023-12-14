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
#include "Config.h"

void    Config::CheckServerSubnodes(const v_node &subcontexts,
                                    ServerConfiguration &current) {
    for (v_node_c_it it = subcontexts.begin(); it != subcontexts.end(); ++it) {
        if (it->IsLocation()) {
            try {
                current.HandleLocationContext(*it);
            } catch (const std::exception &ServerConfigError) {
                ThrowSyntaxError("Location context misconfigured!");
            }
        } else if (it->IsLimitExcept()) {
            ThrowSyntaxError("limit_except block is not allowed here");
        } else {
            ThrowSyntaxError("Only location blocks are allowed inside a server "
                             "context, everything else will be ignored");
        }
    }
}

bool check_filesystem(const std::string &address,
                      const std::string &def_res_address) {
    std::ifstream file((def_res_address + address).c_str());
    if (file.good()) {
        file.close();
        return true;
    }
    file.close();
    return false;
}

/**
 * Checks server node_, and creates a server config based on the content of node_
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
ServerConfiguration Config::CheckServer(Node &node,
                    const std::string &resource_path) {
    ServerConfiguration current;
    try {
        current.ProcessDirectives(node.directives_);
        CheckServerSubnodes(node.child_nodes_, current);
        current.GetRoot().UpdateSublocations();
    } catch (const std::exception &ServerConfigError) {
        ThrowSyntaxError("Server context misconfigured!");
    }
    if (!check_filesystem(current.GetRoot().address_, resource_path))
        ThrowSyntaxError("Root directory doesn't exist");
    return current;
}

bool Config::HasServerWithSameNameOrPort(const ServerConfiguration &config) {
    for (l_srvconf_it_c it = servers_.begin(); it != servers_.end(); ++it) {
        if (it->server_name_ == config.server_name_)
            return true;
        if (it->port_ == config.port_)
            return true;
    }
    return false;
}

/**
 * Checks directives and blocks of a main context, creating server
 * configurations at the same time
 * @param root node_ of a parsed config
 */
void Config::CreateSrvConfigs(Node& root) {
    if (!root.directives_.empty()) {
        std::cout << "Found directive(s) inside main context" << std::endl;
        std::cout << "Only \"server\" blocks are allowed inside a main "
                     "context, everything else will be ignored" << std::endl;
    }
    for (size_t i = 0; i < root.child_nodes_.size(); i++) {
        if (root.child_nodes_[i].main_[0] == "server") {
            ServerConfiguration config = CheckServer(root.child_nodes_[i]);
            if (HasServerWithSameNameOrPort(config))
                ThrowSyntaxError("Server name and port needs to be unique "
                                 "amongst all servers");
            servers_.push_back(config);
        } else {
            std::cout << "Found block " + root.child_nodes_[i].main_[0] + " " +
                         "inside main context" << std::endl;
            std::cout << "Only \"server\" blocks are allowed inside a main "
                         "context, else will be ignored" << std::endl;
        }
    }
    if (servers_.empty()) {
        ThrowSyntaxError("At least one server needs to be defined in the "
                         "main context of a config file");
    }
}
