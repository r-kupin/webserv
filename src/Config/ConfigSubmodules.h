/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSubmodules.h                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/19 23:46:33 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_CONFIGSUBMODULES_H
#define WEBSERV_LIB_CONFIGSUBMODULES_H

#include <string>
#include <vector>
#include <set>
#include <ostream>
#include "Location.h"

/**
 * @brief server pre-configuration
 */
struct ServerConfiguration {
//    server properties
    bool default_index_;
    bool default_hostname_;
    int port_;
    std::string port_str_;
    size_t client_max_body_size_;
    std::string server_name_;
    std::set<std::string > server_names_;
//    secondary locations
    Location root_loc_;

    ServerConfiguration();

    void                UpdateIndex(const v_strings &directive);
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_strings &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_strings &directive);
    void                CheckServerDirectives(std::vector<v_strings> &directives);
    static void         ThrowServerConfigError(const std::string &msg);
    void                UpdateHostname(const v_strings &directives);

    bool operator==(const ServerConfiguration &rhs) const;
	void InheritanceErrPagesRoot(Location &start);
};

/**
 * @brief struct for parsed NGINX block
 * @param main_ directive preceding the block
 * @param directives_ vector of directives (vectors of strings)
 * @param child_nodes_ nested blocks
 */
struct Node {
    v_strings main_;
    std::vector<v_strings> directives_;
    std::vector<Node> child_nodes_;
};

/**
 * @brief struct used only during parsing process - to keep redd
 * leftovers left after block parsing
 */
struct RawNode {
    Node node;
    std::string leftover;
};

#endif //WEBSERV_LIB_CONFIGSUBMODULES_H
