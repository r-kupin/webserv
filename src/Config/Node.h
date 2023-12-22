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

    Node();
    Node(const v_str &main, const std::vector<v_str> &directives);
    Node(const v_str &main, const v_node &childNodes);
    Node(const v_str &main, const std::vector<v_str> &directives,
         const v_node &childNodes);

    bool IsLocation() const;
    bool IsLimitExcept() const;
    bool LocationContextHasPropperAddress() const;
    bool LocationContextIsNotEmpty() const;
    bool ContextDoesntHaveSubLocationsWithSameAddress() const;

    const std::string & LocationContextGetAddress() const;
};

#endif //WEBSERV_LIB_NODE_H
