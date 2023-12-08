/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Config.h                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 03:25:01 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_CONFIGPARSER_H
#define WEBSERV_CONFIGPARSER_H

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <ostream>
#include <list>
#include "ConfigSubmodules.h"

typedef const std::list<ServerConfiguration>            l_srvconf_c;
typedef std::list<ServerConfiguration>::const_iterator  l_srvconf_it_c;

const static std::string kDefaultResPath = "resources/";
const static std::string kDefaultConfig = "resources/default/nginx.conf";

/**
 * @brief struct for parsed NGINX block
 * @param main_ directive preceding the block
 * @param directives_ vector of directives (vectors of strings)
 * @param child_nodes_ nested blocks
 */
struct Node {
    v_str main_;
    std::vector<v_str> directives_;
    std::vector<Node> child_nodes_;
};

/**
 * @brief struct used only during parsing process - to keep redd
 * leftovers left after block parsing
 */
struct RawNode {
    Node node_;
    std::string leftover_;
};

class Config {
public:
//  Exposed to use
    class ConfigException : public std::exception {};

    Config();
    Config(const Config &);
    Config(const std::string &);
    Config &operator=(const Config &);

    //  Exposed to testing
    //  Test constructor only
    explicit    Config(const Node &confRoot);

    ~Config();

    const std::string                       &getConfPath() const;
    const std::list<ServerConfiguration>    &getServers() const;

protected:
//  Config processing utils
    void        ExcludeComments(std::string &line) const;
    void        TrimWhitespaces(std::string &line) const;
//  Parsing config file to tree-like structure of nodes
    void        ParseConfig(std::ifstream &config);
//  Parsing config file to tree-like structure of nodes
    void        CreateSrvConfigs(Node& root);
    //      Location subcontext
    void        HandleLocationContext(Node &loc_context,
                                      ServerConfiguration &sc,
                                      l_loc_it parent);
    //  Limit_except subcontext
//    void        HandleLimitExceptContext(Node &node, Limit &curr_limit) const;
    void        CheckServerSubnodes(Node &node, ServerConfiguration &current);
    ServerConfiguration CheckServer(Node &node,
                                    const std::string &resource_path = kDefaultResPath);
    static bool IsLocation(const Node &node);
    static bool IsLimitExcept(const Node &node);
    bool        IsCorrectLocation(const Node &node);
    bool        IsCorrectLimitExcept(Node &node, Location &current);
private:
    std::string                     conf_path_;
    Node                            conf_root_;
    std::list<ServerConfiguration>  servers_;

//  Parse config
    RawNode             ParseNode(std::ifstream &config,
                                  std::string &line_leftover,
                                  const v_str &main_directive) const;
    void                PreprocessLine(std::string &line,
                                       const std::string &line_leftover) const;
    void                GetChildNode(RawNode &current, std::ifstream &config,
                                     std::string &line) const;
    void                GetDirective(std::string &line, RawNode &current,
                                     std::ifstream &config) const;
    void                FinishSubNode(std::string &line, RawNode &current,
                                      std::ifstream &config) const;
    static v_str        ParseDirective(std::string &line, char c);
    void                HandleLineLeftower(std::string &line_leftover,
                                           std::string &line) const;
    void                FinishMainNode(RawNode &current,
                                       std::ifstream &config) const;
//  Global server check

    void        ThrowSyntaxError(const std::string &msg,
                                 std::ifstream &config) const;
    void        ThrowSyntaxError(const std::string &msg) const;
    bool        LimExIsDefined(const Location &location);
    bool        WillHaveSameAddressAs(Node &node, Location &location);
    Location    &AddOrUpdate(Location &child, Location &parent);
    void        CheckParentDoesntHaveItAlready(Location &current,
                                               Location &parent);
    bool        NeedToAddCurrentToParent(l_loc_it &parent, Location &current,
                                         std::vector<Node>::iterator &it);
    void        HandleSublocation(ServerConfiguration &sc, l_loc_it &parent,
                                  Location &current,
                                  std::vector<Node>::iterator &child);
    void        CheckHTTPMethodsLimitExcept(Node &node,
                                            Limit &curr_limit) const;
    void        CheckDirectivesLimitExcept(const Node &node,
                                           Limit &curr_limit) const;
    void        DenyAddress(const std::string &address,
                            Limit &curr_limit) const;
    void        AllowAddress(const std::string &address,
                             Limit &curr_limit) const;
    bool        HasServerWithSameNameOrPort(const ServerConfiguration &config);
};

std::ostream    &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H