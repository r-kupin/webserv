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
#include "ConfigSubmodules.h"

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

    ~Config();

    const std::string                       &getConfPath() const;
    const std::vector<ServerConfiguration>  &getServers() const;

protected:
//  Exposed to testing
//  Test constructor only
    explicit                            Config(const Node &confRoot);
//  Config processing utils
    void                                ExcludeComments(std::string &line) const;
    void                                TrimWhitespaces(std::string &line) const;
//  Parsing config file to tree-like structure of nodes
    void                                ParseConfig(std::ifstream &config);
//  Parsing config file to tree-like structure of nodes
    std::vector<ServerConfiguration>    CheckComponents(Node& root);
    //      Location subcontext
    void                                HandleLocationContext(Node &loc_context,
                                              ServerConfiguration &sc,
                                              Location &parent);
    //  Limit_except subcontext
    void                                HandleLimitExceptContext(Node &node,
                                                                 Limit &curr_limit) const;
    void                                CheckServerSubnodes(Node &node,
                                                  ServerConfiguration &current);
    void                                CheckServer(std::vector<ServerConfiguration> &servers,
                                                    Node &node);
private:
    std::string conf_path_;
    Node conf_root_;
    std::vector<ServerConfiguration> servers_;

//  Parse config
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
//  Global server check

    static bool         IsLocation(const Node &node);
    static bool         IsCorrectLocation(const Node &node);
    static bool         IsLimitExcept(const Node &node);
    static bool         IsCorrectLimit(const Node &node);
    void                ThrowSyntaxError(const std::string &msg,
                                         std::ifstream &config) const;
    void                ThrowSyntaxError(const std::string &msg) const;
    bool                LimExIsDefined(const Location &location);
};

std::ostream &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H