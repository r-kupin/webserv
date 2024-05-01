/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigCreate.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 20:18:05 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/01 20:06:35 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Include standard and project-specific headers
#include <iostream>
#include <string>
#include <algorithm>
#include "Config.h"

/**
 * Checks directives and blocks of a main context, creating server
 * configurations at the same time.
 * @param root node_ of a parsed config.
 */
void Config::CreateSrvConfigs(Node& root) {
	// Warn about directives inside the main context, which are ignored
	if (!root.directives_.empty()) {
		std::cout << "Found directive(s) inside main context" << std::endl;
		std::cout << "Only \"server\" blocks are allowed inside a main "
					 "context, everything else will be ignored" << std::endl;
	}

	// Iterate over child nodes of the root
	for (size_t i = 0; i < root.child_nodes_.size(); i++) {
		// Check if the node represents a "server" block
		if (root.child_nodes_[i].main_[0] == "server") {
			// Create a new ServerConfiguration and check its validity
			servers_.push_front(ServerConfiguration());
			CheckServer(root.child_nodes_[i], servers_.front());
		} else {
			// Warn about non-server blocks inside the main context
			std::cout << "Found block " + root.child_nodes_[i].main_[0] + " " +
						 "inside main context" << std::endl;
			std::cout << "Only \"server\" blocks are allowed inside a main "
						 "context, else will be ignored" << std::endl;
		}
	}

	// If no server blocks were found, throw a syntax error
	if (servers_.empty()) {
		ThrowSyntaxError("At least one server needs to be defined in the "
						 "main context of a config file");
	}
}

/**
 * Checks server node, and creates a server config based on the content of node.
 * 1. Makes a set to keep track of the crucial parameters of the server.
 *    If we encounter the definition of a parameter - mark corresponding bool flag
 *    as true.
 * 2. Cycle through the subnodes - and add found params to server config.
 * 3. Check directives - and add found params to server config.
 * 4. Check that all crucial parameters are defined.
 * @throw SyntaxError if something is missing.
 * @param node of the server block we are currently checking.
 * @return ready-to-use server configuration.
 */
void Config::CheckServer(Node &node, ServerConfiguration &current) {
	try {
		// Process the server node's directives
		current.ProcessDirectives(node.directives_);
		// Check server's subnodes
		CheckServerSubnodes(node.child_nodes_, current);
		// Update sub-locations after processing the root node
		current.GetRoot().UpdateSublocations();
	} catch (const std::exception &ServerConfigError) {
		// Throw a syntax error if server configuration fails
		ThrowSyntaxError("Server context misconfigured!");
	}
}

/**
 * Check the subnodes of a server, and validate them.
 * @param subcontexts: A vector of child nodes representing server subcontexts.
 * @param current: The current ServerConfiguration being created.
 */
void Config::CheckServerSubnodes(const v_node &subcontexts,
									ServerConfiguration &current) {
	s_str address_set;  // To track unique addresses
	for (v_node_c_it it = subcontexts.begin(); it != subcontexts.end(); ++it) {
		if (it->IsLocation()) {
			// Check location contexts within the server
			CheckLocationContextInServer(current, address_set, it);
		} else if (it->IsLimitExcept()) {
			// Throw an error for illegal "limit_except" blocks
			ThrowSyntaxError("limit_except block is not allowed here");
		} else {
			// Throw an error for any other illegal subnode
			ThrowSyntaxError("Only location blocks are allowed inside a server "
							 "context, everything else will be ignored");
		}
	}
}

/**
 * Checks and processes a location context inside a server configuration.
 * @param current: The current ServerConfiguration being created.
 * @param address_set: A set of addresses to ensure no duplicates.
 * @param it: Iterator pointing to the current node to be checked.
 */
void Config::CheckLocationContextInServer(ServerConfiguration &current,
										  s_str &address_set,
										  v_node_c_it &it) const {
	// Check for duplicate location addresses
	if (address_set.find(it->LocationContextGetAddress()) !=
		address_set.end()) {
		ThrowSyntaxError("Server context can't have subcontexts with "
						 "same address");
	} else {
		// Insert the new address into the set
		address_set.insert(it->LocationContextGetAddress());
		try {
			// Process the location context and add it to the server configuration
			current.HandleLocationContext(*it);
		} catch (const std::exception &ServerConfigError) {
			// Throw a syntax error if the location context is misconfigured
			ThrowSyntaxError("Location context misconfigured!");
		}
	}
}
