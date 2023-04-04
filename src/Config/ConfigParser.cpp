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

bool is_min(size_t n, size_t a, size_t b) {
    if (n < a && n < b)
        return true;
    return false;
}

void Config::ParseConfig(std::ifstream &config) {
    v_strings main;
    main.push_back("main");

    std::string empty = std::string("");
    RawNode root = ParseNode(config, main, empty);
    if (!root.leftower.empty()) {
        ThrowSyntaxError("main block isn't closed!", config);
    }
    conf_root_ = root.node;
    config.close();
}

Config::RawNode
Config::ParseNode(std::ifstream &config,
                  const v_strings &main_directive,
                  std::string &line_leftower) const {
    std::string line;
    RawNode current;

    current.node.main_ = main_directive;
    while (std::getline(config, line) || !line_leftower.empty()) {
        PreprocessLine(line, line_leftower);
        while (!line.empty()) {
            size_t op_br_pos = line.find_first_of('{');
            size_t cl_br_pos = line.find_first_of('}');
            size_t sc_pos = line.find_first_of(';');
            if (op_br_pos != std::string::npos &&
                    is_min(op_br_pos, cl_br_pos, sc_pos))
                GetChildNode(current, config, line);
            if (sc_pos != std::string::npos &&
                    is_min(sc_pos, op_br_pos, cl_br_pos))
                GetDirective(line, current);
            if (cl_br_pos != std::string::npos &&
                    is_min( cl_br_pos, sc_pos, op_br_pos)) {
                FinishNode(line, current);
                return current;
            }
            HandleLineLeftower(line_leftower, line);
        }
    }
    return current;
}

void Config::HandleLineLeftower(std::string &line_leftower,
                                std::string &line) const {
    if (!line.empty()) {
        line_leftower = line;
        line = "";
    } else {
        line_leftower = "";
    }
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
            ParseDirective(line, '{'),
            line);
    current.node.child_nodes_.push_back(child.node);
    line = child.leftower;
}

void Config::GetDirective(std::string &line,
                          Config::RawNode &current) {
    current.node.derectives_.push_back(ParseDirective(line, ';'));
}

void Config::FinishNode(std::string &line, Config::RawNode &current) const {
//    if (!line_leftower.empty()) {
//        ThrowSyntaxError("directive must be terminated with \";\" "
//                         "before termination of the block!", config);
//    }
    if (line.size() > 1) {
        line = line.substr(line.find_first_of('}') + 1);
        current.leftower = line;
    } else {
        current.leftower = "";
    }
}

v_strings Config::ParseDirective(std::string &line, char c) {
    v_strings params;
    std::string all_separators = " \t";
    all_separators.push_back(c);
    while (line[0] != c) {
        size_t separator = line.find_first_of(all_separators);
        params.push_back(line.substr(0, separator));
        line = line.substr(separator);
        line = line.substr(line.find_first_not_of(" \t"));
    }
    line = line.substr(1);
    return params;
}