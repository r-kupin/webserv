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
    class ConfigException : public std::exception {};

    Config();
    Config(const Config &);
    Config(const std::string &);
    explicit    Config(const Node &confRoot);
    Config &operator=(const Config &);

    ~Config();

    const std::string   &getConfPath() const;
    const l_sc          &getConstServers() const;
protected:
//-------------------parsing config filestream to the tree of nodes-------------
    void                ParseConfig(std::ifstream &config);
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
//-------------------creating the list of server configurations-----------------
    void                CreateSrvConfigs(Node& root);
    void                CheckServer(Node &node, ServerConfiguration &current);
    bool                HasServerWithSameNameOrPort(const ServerConfiguration &config);
    void                CheckServerSubnodes(const v_node &subcontexts,
                                            ServerConfiguration &current);
    void                CheckLocationContextInServer(ServerConfiguration &current,
                                                     std::set<std::string> &address_set,
                                                     v_node_c_it &it) const;
//-------------------getters, exceptions, etc-----------------------------------
    l_sc                &getServers();
    void                ThrowSyntaxError(const std::string &msg,
                                                     std::ifstream &config) const;
    void                ThrowSyntaxError(const std::string &msg) const;
private:
    std::string         conf_path_;
    Node                conf_root_;
    l_sc                servers_;
};

std::ostream    &operator<<(std::ostream &os, const Config &config);

#endif //WEBSERV_CONFIGPARSER_H