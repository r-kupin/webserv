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

typedef std::vector<std::string> v_strings;

enum Methods {GET, POST, DELETE};

struct ErrPage {
    std::string address_;
    int code_;
    ErrPage(const std::string &address, int code);

    bool operator<(const ErrPage &rhs) const;
    bool operator==(const ErrPage &rhs) const;
};
std::ostream &operator<<(std::ostream &os, const ErrPage &page);


struct Limit {
    std::set<Methods> except_;
    int return_code_;

    Limit();

    bool operator==(const Limit &rhs) const;
};
std::ostream &operator<<(std::ostream &os, const Limit &limit);

struct Location {
    std::set<ErrPage> error_pages_;
    std::set<Location> sublocations_;
    std::set<std::string> index_;
    Limit limit_except_;
    int return_code_;
    int default_index_;
    int autoindex_;
    std::string return_address_;
    std::string root_;
    std::string address_;

    Location();
    explicit Location(const std::string &address);
    explicit Location(const std::string &address,
                      const std::string &super_root);
    bool HasSameAddress(const Location &rhs) const;
    bool operator<(const Location &rhs) const;
    bool operator==(const Location &rhs) const;
};
std::ostream &operator<<(std::ostream &os, const Location &location);

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
//    secondary locations
    Location root_loc_;

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
