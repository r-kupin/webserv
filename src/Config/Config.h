/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Config.h                                     :+:      :+:    :+:  */
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
#include "ConfigSubmodules.h"

const static std::string kDefaultResPath = "resources/";
const static std::string kDefaultConfig = "resources/default/nginx.conf";

class Config {
public:
//  Exposed to use
    class ConfigException : public std::exception {};

    Config();
    Config(const Config &);
    Config(const std::string &config_path);
    Config &operator=(const Config &);

    ~Config();

    const std::string &getConfPath() const;
    const std::vector<ServerConfiguration> &getServers() const;

protected:
//  Exposed to testing
//    Test constructor only
    explicit            Config(const Node &confRoot);

//    Config processing utils
    void                ThrowSyntaxError(const std::string &msg,
                                         std::ifstream &config) const;
    void                ThrowSyntaxError(const std::string &msg) const;
    void                ExcludeComments(std::string &line) const;
    void                TrimWhitespaces(std::string &line) const;
    bool                MarkDefined(const std::string &key, bool &flag,
                                    const v_strings &directive) const;
    bool                UMarkDefined(const std::string &key, bool &flag,
                                    const v_strings &directive) const;
    bool                IsNumber(const std::string& str) const;
//      Parsing config file to tree-like structure of nodes
    void                ParseConfig(std::ifstream &config);
//      Parsing config file to tree-like structure of nodes
    std::vector<ServerConfiguration> CheckComponents(Node& root);
private:
    std::string conf_path_;
    Node conf_root_;
    std::vector<ServerConfiguration> servers_;

//      Parse config
    RawNode             ParseNode(std::ifstream &config,
                                  std::string &line_leftover,
                                  const v_strings &main_directive) const;
    void                PreprocessLine(std::string &line,
                                       const std::string &line_leftover) const;
    void                GetChildNode(RawNode &current, std::ifstream &config,
                                     std::string &line) const;
    void                GetDirective(std::string &line, RawNode &current,
                                     std::ifstream &config) const;
    void                FinishSubNode(std::string &line, RawNode &current,
                                      std::ifstream &config) const;
    static v_strings    ParseDirective(std::string &line, char c);
    void                HandleLineLeftower(std::string &line_leftover,
                                           std::string &line) const;
    void                FinishMainNode(RawNode &current,
                                       std::ifstream &config) const;
//      Global server check
    void                CheckServerDirectives(Node &node, bool &port,
                                              ServerConfiguration &current);
    void CheckServer(Node &node, std::vector<ServerConfiguration> &servers);
//      Location subcontext
    void CheckLocation(Node &loc_node, Location &current_l);
    void CheckLocationDirectives(const Node &loc_node, Location &current_l,
                                 bool &set_root, bool &set_index, bool &ret,
                                 bool &err_pages);
    void                HandleLocationReturn(const Node &node,
                                             Location &current_l,
                                             size_t i) const;
//      Limit_except subcontext
    void HandleLimitExceptContext(ConfigNode &node, Limit &curr_limit) const;

    void HandleLocationContext(Node &loc_context,
                               Location &parent);

    void AddErrorPages(const v_strings &directive, Location &location);

    void CheckLocationDirectives(Node &loc_node, Location &current_l);

    static bool IsLocation(const Node &node);

    static bool IsCorrectLocation(const Node &node);

    static bool IsLimitExcept(const Node &node);

    static bool IsCorrectLimit(const Node &node);

    void
    CheckSieblingsAdresses(const Location &parent,
                           const Location &maybe_current) const;

    void UpdateIndex(const v_strings &directive, Location &location);
};

std::ostream &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H