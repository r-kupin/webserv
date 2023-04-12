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

typedef std::vector<std::string> v_strings;

class Config {
public:
    class ConfigException : public std::exception {};

    Config();
    Config(const Config &);
    Config(const std::string &config_path);
    Config &operator=(const Config &);

    ~Config();

protected:
    /**
     * @brief struct for parsed NGINX block
     * @param main_ directive preceding the block
     * @param directives_ vector of directives (vectors of strings)
     * @param child_nodes_ - nested blocks
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
        Node node;
        std::string leftover;
    };
//    Test only
    explicit            Config(const Node &confRoot);

//    Config processing utils
    void                ThrowSyntaxError(const std::string &msg,
                                         std::ifstream &config) const;
    void                ExcludeComments(std::string &line) const;
    void                TrimWhitespaces(std::string &line) const;
//    Parse config
    static v_strings    ParseDirective(std::string &line, char c);
    void                FinishSubNode(std::string &line,
                                      Config::RawNode &current,
                                      std::ifstream &config) const;
    void                GetDirective(std::string &line,
                                     Config::RawNode &current,
                                     std::ifstream &config) const;
    void                GetChildNode(RawNode &current, std::ifstream &config,
                                     std::string &line) const;
    void                PreprocessLine(std::string &line,
                                       const std::string &line_leftover) const;
    Config::RawNode     ParseNode(std::ifstream &config,
                                  const v_strings &main_directive,
                                  std::string &line_leftover) const;
    void                CheckComponents(Node& root);
private:
    std::string conf_path_;
    Node conf_root_;
    int servers_;

    void                ParseConfig(std::ifstream &config);
    void                HandleLineLeftower(std::string &line_leftover,
                                           std::string &line) const;
    void                FinishMainNode(RawNode &current,
                                       std::ifstream &config) const;
    void                CheckServer(Node &node);
    void                ThrowSyntaxError(const std::string &msg) const;
    void                CheckServerDirectives( Node &node,  bool *set) const;

    void CheckLocationContext(Config::Node &node, bool &set_root,
                              bool &set_index) const;

    void CheckLimitExceptContext(Config::ConfigNode &node, bool &set_ret) const;
};


#endif //WEBSERV_CONFIGPARSER_H