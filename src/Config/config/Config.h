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
#include "../server_configuration/ServerConfiguration.h"

typedef std::list<ServerConfiguration>::const_iterator  l_srvconf_it_c;

const static std::string kDefaultResPath = "resources/";
const static std::string kDefaultConfig = "resources/default/nginx.conf";

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
//  Parsing config file to tree-like structure of nodes
    void        ParseConfig(std::ifstream &config);
//  Parsing config file to tree-like structure of nodes
    void        CreateSrvConfigs(Node& root);
//  Location subcontext
    void        CheckServerSubnodes(const v_node &subcontexts, ServerConfiguration &current);
    void        CheckServer(Node &node, ServerConfiguration &current);
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

    bool        HasServerWithSameNameOrPort(const ServerConfiguration &config);
};

std::ostream    &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H