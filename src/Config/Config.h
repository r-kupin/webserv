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
    typedef struct ConfigNode {
        v_strings main_;
        std::vector<v_strings> derectives_;
        std::vector<ConfigNode> child_nodes_;
    } Node;

    struct RawNode {
        Node node;
        std::string leftower;
    };

//    Config processing utils
    void    ThrowSyntaxError(const std::string &msg, std::ifstream &config) const;
    void    ExcludeComments(std::string &line) const;
    void    TrimWhitespaces(std::string &line) const;
//    Check braces
    void CheckSyntax();
//    Parse config
    static v_strings ParseDirective(std::string &line, char endline) ;
    RawNode ParseNode(std::ifstream &config,
                      const std::vector<std::string>& main_directive) const;
    void GetChildNode(RawNode &current,
                      std::ifstream &config,
                      std::string &line) const;
    static void GetDirective(std::string &line,
                      RawNode &current) ;
    void FinishNode(std::ifstream &config,
                    std::string &line,
                    const std::string &line_leftower,
                    RawNode &current) const;
private:
    std::string conf_path_;
    Node conf_root_;


    void ParseConfig(std::ifstream &config);

    void
    PreprocessLine(std::string &line, const std::string &line_leftower) const;
};


#endif //WEBSERV_CONFIGPARSER_H