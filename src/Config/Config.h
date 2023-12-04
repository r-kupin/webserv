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

typedef const std::list<ServerConfiguration> l_srvconf;

typedef std::list<ServerConfiguration>::const_iterator l_srvconf_it_c;
const static std::string kDefaultResPath = "resources/";
const static std::string kDefaultConfig = "resources/default/nginx.conf";

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
    explicit                            Config(const Node &confRoot);

    ~Config();

    const std::string                       &getConfPath() const;
    const std::list<ServerConfiguration>  &getServers() const;

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
    void        HandleLimitExceptContext(Node &node, Limit &curr_limit) const;
    void        CheckServerSubnodes(Node &node, ServerConfiguration &current);
    ServerConfiguration CheckServer(Node &node);
    static bool IsLocation(const Node &node);
    static bool IsLimitExcept(const Node &node);
    bool        IsCorrectLocation(const Node &node);
    bool        IsCorrectLimitExcept(Node &node, Location &current);
private:
    std::string conf_path_;
    Node conf_root_;
    std::list<ServerConfiguration> servers_;

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
    static v_str    ParseDirective(std::string &line, char c);
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
                                  std::vector<Node>::iterator &it);

    void CheckHTTPMethodsLimitExcept(Node &node, Limit &curr_limit) const;

    void CheckDirectivesLimitExcept(const Node &node, Limit &curr_limit) const;

    void deny_address(const std::string &address, Limit &curr_limit) const;

    void allow_address(const std::string &address, Limit &curr_limit) const;

    bool HasServerWithSameNameOrPort(const ServerConfiguration &config);
};

std::ostream    &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H