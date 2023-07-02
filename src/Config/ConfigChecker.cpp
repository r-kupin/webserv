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

void Config::HandleLocationContext(Node &loc_context, ServerConfiguration &sc,
                                   Location &parent) {
    if (!IsCorrectLocation(loc_context))
        ThrowSyntaxError("Location is incorrect");

    Location    maybe_current(loc_context.main_[1]);

    if (!maybe_current.HasSameAddressAsOneOfSublocationsOf(parent)) {
        Location &current =
            (maybe_current.HasSameAddressAs(parent)) ? parent : maybe_current;

        current.CheckLocationDirectives(loc_context.directives_, sc);
        for (std::vector<Node>::iterator it = loc_context.child_nodes_.begin();
             it != loc_context.child_nodes_.end(); ++it) {
            if (IsLocation(*it)) {
                if (!IsCorrectLocation(*it))
                    ThrowSyntaxError("Location is incorrect");
                HandleLocationContext(*it, sc, current);
            } else if (IsLimitExcept(*it)) {
                if (LimExIsDefined(maybe_current))
                    ThrowSyntaxError("Limit except is incorrect");
                HandleLimitExceptContext(*it, current.limit_except_);
            }
        }
        if (!(current.HasSameAddressAs(parent)))
            parent.sublocations_.push_back(current);
    } else {
        ThrowSyntaxError("Each location needs unique address inside each "
                         "context");
    }
}

bool Config::LimExIsDefined(const Location &location) {
    if (location.limit_except_.return_code_ != -1)
        return true;
    return false;
}

void    Config::CheckServerSubnodes(Node &node, ServerConfiguration &current) {
    for (size_t i = 0; i < node.child_nodes_.size(); i++) {
        if (IsLocation(node.child_nodes_[i])) {
            if (!IsCorrectLocation(node.child_nodes_[i]))
                ThrowSyntaxError("Location path is incorrect or missing");
            try {
                HandleLocationContext(node.child_nodes_[i], current,
                                      current.root_loc_);
            } catch (const std::exception &) {
                ThrowSyntaxError("Location block is corrupted");
            }
        } else if (node.child_nodes_[i].main_[0] == "limit_except") {
            ThrowSyntaxError("limit_except block is not allowed here");
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
void    Config::CheckServer(std::vector<ServerConfiguration> &servers,
                            Node &node) {
    ServerConfiguration current;

    current.CheckServerDirectives(node.directives_);
    CheckServerSubnodes(node, current);
    for (std::vector<ServerConfiguration>::iterator it = servers.begin();
													it != servers.end(); ++it) {
        if (it->port_ == current.port_)
            ThrowSyntaxError("Port needs to be unique amongst all servers");
    }
    current.InheritanceErrPagesRoot(current.root_loc_, current.root_loc_);
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
            CheckServer(servers, root.child_nodes_[i]);
        } else {
            std::cout << "Found block " + root.child_nodes_[i].main_[0] + " " +
                         "inside main context" << std::endl;
            std::cout << "Only \"server\" blocks are allowed inside a main "
                         "context, else will be ignored" << std::endl;
        }
    }
    if (servers.empty()) {
        ThrowSyntaxError("At least one server needs to be defined in the "
                         "main context of a config file");
    }
    return servers;
}
