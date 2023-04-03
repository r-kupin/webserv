/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigParser.cpp                                   :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/04 00:50:20 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "Config.h"

void Config::ParseConfig(std::ifstream &config) {
    v_strings main;
    main.push_back("main");

    RawNode root = ParseNode(config, main);
    if (!root.leftower.empty()) {
        ThrowSyntaxError("main block isn't closed!", config);
    }
    conf_root_ = root.node;
}

Config::RawNode Config::ParseNode(std::ifstream &config,
                                  const v_strings& main_directive) const {
    std::string line;
    std::string line_leftower;
    RawNode current;

    current.node.main_ = main_directive;
    while (std::getline(config, line)) {
        PreprocessLine(line, line_leftower);
        while (!line.empty()) {
            size_t op_br_pos = line.find_first_of('{');
            size_t cl_br_pos = line.find_first_of('}');
            size_t sc_pos = line.find_first_of(';');
            if (op_br_pos != std::string::npos)
                GetChildNode(current, config, line);
            if (sc_pos != std::string::npos)
                GetDirective(line, current);
            if (cl_br_pos != std::string::npos) {
                FinishNode(config, line, line_leftower, current);
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

void Config::PreprocessLine(std::string &line,
                       const std::string &line_leftower) const {
    line = line_leftower + " " + line;
    ExcludeComments(line);
    TrimWhitespaces(line);
}

void Config::GetChildNode(Config::RawNode &current,
                          std::ifstream &config,
                          std::string &line) const {
    const RawNode &child = ParseNode(
            config,
            ParseDirective(line, '{'));
    current.node.child_nodes_.push_back(child.node);
    line = child.leftower;
}

void Config::GetDirective(std::string &line,
                          Config::RawNode &current) {
    current.node.derectives_.push_back(ParseDirective(line, ';'));
}

void Config::FinishNode(std::ifstream &config,
                        std::string &line,
                        const std::string &line_leftower,
                        Config::RawNode &current) const {
    if (!line_leftower.empty()) {
        ThrowSyntaxError("directive must be terminated with \";\" "
                         "before termination of the block!", config);
    }
    if (line.size() > 1) {
        line = line.substr(line.find_first_of('}') + 1);
        current.leftower = line;
    } else {
        current.leftower = "";
    }
}

v_strings Config::ParseDirective(std::string &line, char endline) {
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