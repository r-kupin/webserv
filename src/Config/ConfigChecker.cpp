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

bool Config::LimExIsDefined(const Location &location) {
    if (location.limit_except_.except_.empty())
        return false;
    return true;
}

bool Config::WillHaveSameAddressAs(Node &node, Location &location) {
    return node.main_[1] == location.address_;
}

Location &Config::AddOrUpdate(Location &child, Location &parent) {
    return (child.HasSameAddressAs(parent)) ? parent : child;
}

void
Config::CheckParentDoesntHaveItAlready(Location &current, Location &parent) {
    if (!current.HasSameAddressAs(parent) &&
        current.HasSameAddressAsOneOfSublocationsOf(parent)) {
// todo: or maybe we can update non-root locations?
            ThrowSyntaxError("Each location needs unique address inside"
                             "each context");
    }
}

bool Config::NeedToAddCurrentToParent(l_loc_it &parent, Location &current,
                                      std::vector<Node>::iterator &it) {
    return !WillHaveSameAddressAs(*it, current) &&
           parent->address_ != current.address_ &&
           !parent->HasAsSublocation(current);
}

void Config::HandleSublocation(ServerConfiguration &sc, l_loc_it &parent,
                               Location &current,
                               std::vector<Node>::iterator &child) {
    if (NeedToAddCurrentToParent(parent, current, child))
        parent->sublocations_.push_front(current);
    if (parent->HasSameAddressAs(current)) {
        HandleLocationContext(*child, sc, parent);
    } else {
        HandleLocationContext(*child, sc, parent->sublocations_.begin());
    }
}

void Config::HandleLocationContext(Node &loc_context,
                                   ServerConfiguration &sc,
                                   l_loc_it parent) {
    Location maybe_current;

    try {
        maybe_current = Location(loc_context.main_[1], parent);
    } catch (Location::LocationException &) {
        ThrowSyntaxError("Location address contains invalid characters");
    }

    CheckParentDoesntHaveItAlready(maybe_current, *parent);
    Location &current = AddOrUpdate(maybe_current, *parent);

    current.ProcessDirectives(loc_context.directives_);
    for (std::vector<Node>::iterator it = loc_context.child_nodes_.begin();
                                it != loc_context.child_nodes_.end(); ++it) {
        if (IsCorrectLimitExcept(*it, current)) {
            current.HandleLimitExcept(it->main_, it->directives_);
        } else if (IsCorrectLocation(*it)) {
            HandleSublocation(sc, parent, current, it);
        }
    }
    if (!parent->HasSameAddressAs(current) &&
        !parent->HasAsSublocation(current))
        parent->sublocations_.push_front(current);
}

void    Config::CheckServerSubnodes(Node &node, ServerConfiguration &current) {
    for (size_t i = 0; i < node.child_nodes_.size(); i++) {
        if (IsCorrectLocation(node.child_nodes_[i])) {
            try {
                HandleLocationContext(node.child_nodes_[i], current,
                                      current.locations_.begin());
            } catch (const std::exception &) {
                ThrowSyntaxError("Location block is corrupted");
            }
        } else if (node.child_nodes_[i].main_[0] == "limit_except") {
            ThrowSyntaxError("limit_except block is not allowed here");
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

    current.CheckServerDirectives(node.directives_);
    CheckServerSubnodes(node, current);
    for (l_srvconf_it_c it = servers_.begin(); it != servers_.end(); ++it) {
        if (it->port_ == current.port_)
            ThrowSyntaxError("Port needs to be unique amongst all servers");
    }
    current.GetRoot().UpdateSublocations();
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
