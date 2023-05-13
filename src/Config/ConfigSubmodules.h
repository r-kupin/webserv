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

typedef std::vector<std::string> v_strings;

enum Methods {GET, POST, DELETE};
enum Action {DENY};

struct ErrPage {
    std::string address_;
    std::vector<int> code_;


    ErrPage(const std::string &address, const std::vector<int> &code);

    explicit ErrPage(const std::string &address);

    bool operator==(const ErrPage &rhs) const;
};

struct Location {
    std::set<Methods> limit_except_methods_;
    std::vector<ErrPage> error_pages_;
    Action limit_except_action_;
    int limit_except_return_code_;
    v_strings index_;
    int return_code_;
    std::string return_address_;
    std::string root_;
    std::string address_;

    Location();

    explicit Location(const std::string &address);

    bool operator==(const Location &rhs) const;
};

/**
 * @brief server pre-configuration
 */
struct ServerConfiguration {
//    server properties
    int port_;
    std::string port_str_;
    size_t client_max_body_size_;
    std::string hostname_;
    v_strings server_names_;
    bool explicit_default_location_set_;
//    secondary locations
    std::vector<Location> locations_;

    ServerConfiguration();

    bool operator==(const ServerConfiguration &rhs) const;
};


/**
 * @brief struct for parsed NGINX block
 * @param main_ directive preceding the block
 * @param directives_ vector of directives (vectors of strings)
 * @param child_nodes_ nested blocks
 */
typedef struct ConfigNode {
    v_strings main_;
    std::vector<v_strings> directives_;
    std::vector<ConfigNode> child_nodes_;
} Node;

/**
 * @brief struct used only during parsing process - to keep redd
 * leftovers left after block parsing
 */
struct RawNode {
    ConfigNode node;
    std::string leftover;
};

#endif //WEBSERV_LIB_CONFIGSUBMODULES_H
