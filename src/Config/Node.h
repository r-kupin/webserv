#ifndef WEBSERV_LIB_NODE_H
#define WEBSERV_LIB_NODE_H

#include <vector>
#include <string>

struct Node;

typedef std::vector<Node>                           v_node;
typedef std::vector<Node>::const_iterator           v_node_c_it;
typedef std::vector<std::string>                    v_str;
typedef std::vector<std::string>::const_iterator    v_str_c_it;
typedef std::vector<v_str>::const_iterator          vstr_vstr_c_it;

/**
 * @brief struct for parsed NGINX block
 * @param main_ directive preceding the block
 * @param directives_ vector of directives (vectors of strings)
 * @param child_nodes_ nested blocks
 */
struct Node {
    v_str main_;
    std::vector<v_str> directives_;
    v_node child_nodes_;

    bool IsLocation() const;
    bool IsLimitExcept() const;
    bool LocationContextHasPropperAddress() const;
    bool LocationContextIsNotEmpty() const;
    const std::string & LocationContextGetAddress() const;
};

#endif //WEBSERV_LIB_NODE_H
