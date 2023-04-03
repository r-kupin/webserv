/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSyntaxCheck.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/29 13:18:50 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "Config.h"
#include "ConfigExceptions.h"

void    SyntaxError(const std::string &msg, std::ifstream &config) {
    std::cout << "Syntax error: " + msg << std::endl;
    config.close();
    throw ConfigFileSyntaxError();
}

void    ExcludeComments(std::string &line) {
    if (!line.empty()) {
        size_t comment_position = line.find('#');
        if (comment_position != std::string::npos) {
            line = line.substr(0, comment_position);
        }
    }
}

/**
 * @brief simple nginx syntax checker - checks correctness of braces
 * @param config opened input stream to read config from
 */
void Config::CheckSyntax() {
    std::ifstream config;
    config.open(conf_path_.c_str());

    std::string line;
    int open_braces = 0;
    while (std::getline(config, line)) {
        ExcludeComments(line);
//      checking braces
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '{') {
                ++open_braces;
            } else if (line[i] == '}') {
                if (!open_braces) {
                    SyntaxError("missing '{'", config);
                } else {
                    --open_braces;
                }
            }
        }
    }
    if (open_braces)
        SyntaxError("missing '}'", config);
    config.close();
}

void    TrimWhitespaces(std::string &line) {
    if (!line.empty()) {
        size_t whitespaces_before = line.find_first_not_of(" \t\n");
        size_t whitespaces_after = line.find_last_not_of(" \t\n");
        if (whitespaces_before != std::string::npos &&
            whitespaces_after != std::string::npos )
            line = line.substr(whitespaces_before,
                           whitespaces_after - whitespaces_before + 1);
        else
            line = "";
    }
}

v_strings ParseDirective(std::string &line, char endline) {
    v_strings params;
    std::string all_separators = " \t";
    all_separators.push_back(endline);
    while (line[0] != endline) {
        size_t separator = line.find_first_of(all_separators);
        params.push_back(line.substr(0, separator));
        line = line.substr(separator);
        line = line.substr(line.find_first_not_of(" \t"));
    }
    line = line.substr(1);
    return params;
}

Config::RawNode Config::ParseNode(std::ifstream &config,
                                  const v_strings& main_directive) {
    std::string line;
    std::string line_leftower;
    RawNode current;

    current.node.main_ = main_directive;
    while (std::getline(config, line)) {
        line = line_leftower + " " + line;
        ExcludeComments(line);
        TrimWhitespaces(line);
        while (!line.empty()) {
            size_t op_br_pos = line.find_first_of('{');
            size_t cl_br_pos = line.find_first_of('}');
            size_t sc_pos = line.find_first_of(';');
            if (op_br_pos != std::string::npos) {
                const RawNode &child = ParseNode(
                        config,
                        ParseDirective(line, '{'));
                current.node.child_nodes_.push_back(child.node);
                line = child.leftower;
            }
            if (sc_pos != std::string::npos) {
                current.node.derectives_.push_back(ParseDirective(line, ';'));
            }
            if (cl_br_pos != std::string::npos) {
                if (!line_leftower.empty()) {
                    SyntaxError("directive must be terminated with \";\" "
                                "before termination of the block!", config);
                }
                if (line.size() > 1) {
                    line = line.substr(line.find_first_of('}') + 1);
                    current.leftower = line;
                } else {
                    current.leftower = "";
                }
                return current;
            }
            if (line.find_first_of('{')== std::string::npos &&
                line.find_first_of(';')== std::string::npos) {
                line_leftower = line;
                line = "";
            }
        }
    }
    return current;
}

void Config::ParseConfig(std::ifstream &config) {
    v_strings main;
    main.push_back("main");

    RawNode root = ParseNode(config, main);
    if (!root.leftower.empty()) {
        SyntaxError("main block isn't closed!", config);
    }
    conf_root_ = root.node;
}

//      OLD
//void ParseDirective(std::string &line, v_strings &params, char endline) {
//    std::string all_separators = " \t";
//    all_separators.push_back(endline);
//    while (line[0] != endline) {
//        size_t separator = line.find_first_of(all_separators);
//        params.push_back(line.substr(0, separator));
//        line = line.substr(separator);
//        line = line.substr(line.find_first_not_of(" \t"));
//    }
//    line = line.substr(1);
//}
//      OLD
//void Config::Evaluate(std::ifstream &config) {
//    std::string line;
//    std::string line_leftower;
//    Node &current = conf_root_;
//
//    current.main_.push_back("config_root");
//    while (std::getline(config, line)) {
//        line = line_leftower + " " + line;
//        ExcludeComments(line);
//        TrimWhitespaces(line);
//        while (!line.empty()) {
//            size_t br_pos = line.find_first_of('{');
//            size_t sc_pos = line.find_first_of(';');
//            if (br_pos != std::string::npos) {
//                Node subnode;
//                ParseDirective(line, subnode.main_, '{');
//
//            }
//            if (sc_pos != std::string::npos) {
//                v_strings params;
//                ParseDirective(line, params, ';');
//                current.derectives_.push_back(params);
//            }
//            if (line.find_first_of('{')== std::string::npos &&
//                    line.find_first_of(';')== std::string::npos) {
//                line_leftower = line;
//                line = "";
//            }
//        }
//    }
//}








//-------------------------------------------------------old
//enum OP { StartBlock, EndBlock, Content };
//
// while (std::getline(config, line)) {
//
////      excluding comments
//        unsigned long comment_position = line.find('#');
//        if (comment_position != std::string::npos)
//            line = line.substr(0, comment_position);
//
//        if (!line.empty()) {
////          trim whitespaces before & after line content
//            unsigned long whitespaces_before = line.find_first_not_of(" \t\n");
//            unsigned long whitespaces_after = line.find_last_not_of(" \t\n");
//            line = line.substr(whitespaces_before,
//                               whitespaces_after - whitespaces_before + 1);
////          end of block
//            if (line == "}") {
////              fail if there were no beginning
//                if (stack.empty()) {
//                    std::cout << "Syntax error: missing '{'" << std::endl;
//                    throw ConfigFileSyntaxError();
//                }
////              replace block content
//                stack.pop();
//                current = EndBlock;
//            } else if (line.find('{') != std::string::npos) {
//                stack.push(current);
//            } else if (line.find(';') == std::string::npos) {
//                std::cout << "Syntax error: missing ';'" << std::endl;
//                throw ConfigFileSyntaxError();
//            } else {
//                current = BlockContent;
//            }
//        }
//    }
//    if (!stack.empty()) {
//        std::cout << "Syntax error: missing '}' at end of file" << std::endl;
//        throw ConfigFileSyntaxError();
//    }

/**
 * @brief Asks builtin NGINX to check config syntax
 * @note Unusable, cause requires sudo
 * @param config absolute path to config file
 */
//void Config::CheckSyntaxWithNginxTester(const std::string &path) {
//    char * const argv[] = {
//            const_cast<char *>("sudo"),         /* impossible */
//            const_cast<char *>("nginx"),        /* ugly */
//            const_cast<char *>("-c"),           /* ugly */
//            const_cast<char *>(path.c_str()),   /* ugly */
//            const_cast<char *>("-t"),           /* ugly */
//            NULL};
//    pid_t pid = fork();
//    int status;
//
//    if (pid == 0) {
//        execve("./", argv, environ);
//        std::cout << "Check with nginx builtin tester failed!" << std::endl;
//        exit(1);
//    } else if (pid > 0) {
//        waitpid(pid, &status, 0);
//        if (status != 0) {
//            std::cout << "Check with nginx builtin tester failed!" << std::endl;
//            throw ConfigFileSyntaxError();
//        }
//    } else {
//        std::cout << "fork() failed!" << std::endl;
//    }
//}
