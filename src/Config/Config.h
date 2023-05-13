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
                                              ServerConfiguration &current) const;
    void CheckServer(Node &node, std::vector<ServerConfiguration> &servers);
//      Location subcontext
    void CheckLocation(Node &loc_node, Location &current_l);
    void CheckLocationDirectives(const Node &loc_node, Location &current_l,
                                 bool &set_root, bool &set_index, bool &ret,
                                 bool &err_pages) const;
    void                HandleLocationReturn(const Node &node,
                                             Location &current_l,
                                             size_t i) const;
//      Limit_except subcontext
    void CheckLimitExceptContext(ConfigNode &node, Location &location,
                                 bool &limit) const;

    void                HandleServerContext(ConfigNode &srv_node,
                                            std::vector<ServerConfiguration>
                                                                    &servers);

    void HandleLocationContext(Node &maybe_loc_context,
                               ServerConfiguration &current_srv);

    void DefaultLocationDirective(ServerConfiguration &current) const;

    void AddErrorPages(const v_strings &directive, Location &location) const;
};

std::ostream &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H