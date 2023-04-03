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

private:
    typedef struct ConfigNode {
        v_strings main_;
        std::vector<v_strings> derectives_;
        std::vector<ConfigNode> child_nodes_;
    } Node;

    struct RawNode {
        Node node;
        std::string leftower;
    };

    std::string conf_path_;
    Node conf_root_;

    void CheckSyntax();
//    void Evaluate(std::ifstream &config);
    RawNode ParseNode(std::ifstream &config, const std::vector<std::string>& main_directive);
    void ParseConfig(std::ifstream &config);
};


#endif //WEBSERV_CONFIGPARSER_H