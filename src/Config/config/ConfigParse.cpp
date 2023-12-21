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

void    exclude_comments(std::string &line) {
    if (!line.empty()) {
        size_t comment_position = line.find('#');
        if (comment_position != std::string::npos) {
            line = line.substr(0, comment_position);
        }
    }
}

void    trim_whitespaces(std::string &line) {
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

bool is_min(size_t n, size_t a, size_t b) {
    if (n < a && n < b)
        return true;
    return false;
}

/**
 * @brief recursive config parser
 * Creates a main node_ out of main block, then goes through directives and
 * explicit sub-nodes
 * @param config input stream
 */
void Config::ParseConfig(std::ifstream &config) {
    v_str main;
    main.push_back("main");

    std::string empty = std::string("");
    RawNode root = ParseNode(config, empty, main);
    if (!root.leftover_.empty()) {
        ThrowSyntaxError("main block isn't closed!", config);
    }
    conf_root_ = root.node_;
}

/**
 * @brief Parses a NGINX block into a config node_
 *  NGINX block is one or many lines that has a main directive (array of
 * strings) and a block body - directives and sub-blocks between '{' and '}'.
 *  NGINX blocks are used to group together directives and define
 * configuration contexts within the NGINX configuration file. Each block is
 * defined using a pair of curly braces {} and contains one or more
 * directives or nested blocks.
 * There are two types of blocks in NGINX:
 *  The main block - This is the top-level block in the configuration file,
 * and it contains directives that apply to the entire server.
 *  Context blocks - These are nested within the main block and contain
 * directives that apply to specific contexts within the server, such as a
 * specific server, location, or upstream.
 * This method at first assigns main directive of block being parsed
 * Goes through config or lefter line left from previous nodes
 * Founds first occurrence of '{', ';' and/or '}'
 *  if ';' - extracts directive
 *  if '{' - recursively parses sub node_
 *  if '}' - end of current (sub)node_
 * Saves unparsed line leftovers for next iterations
 * @param config input stream
 * @param main_directive directive that was preceding current block
 * @param line_leftover unparsed leftovers from previous getline() call
 * @return parsed node_ with saved leftovers
 */
RawNode
Config::ParseNode(std::ifstream &config, std::string &line_leftover,
                  const v_str &main_directive) const {
    std::string line;
    RawNode current;

    current.node_.main_ = main_directive;
    while (std::getline(config, line) || !line_leftover.empty()) {
        PreprocessLine(line, line_leftover);
        while (!line.empty()) {
            size_t op_br_pos = line.find_first_of('{');
            size_t cl_br_pos = line.find_first_of('}');
            size_t sc_pos = line.find_first_of(';');
            if (op_br_pos != std::string::npos &&
                    is_min(op_br_pos, cl_br_pos, sc_pos))
                GetChildNode(current, config, line);
            if (sc_pos != std::string::npos &&
                    is_min(sc_pos, op_br_pos, cl_br_pos))
                GetDirective(line, current, config);
            if (cl_br_pos != std::string::npos &&
                    is_min( cl_br_pos, sc_pos, op_br_pos)) {
                FinishSubNode(line, current, config);
                return current;
            }
            HandleLineLeftower(line_leftover, line);
        }
    }
    FinishMainNode(current, config);
    return current;
}

/**
 * @brief Prepares line to be parsed
 * adds newly redden line to what's left
 * Removes commented parts
 * Cuts leading and following whitespaces
 * @param line Line from getline()
 * @param line_leftover unparsed leftovers from previous getline() call
 */
void Config::PreprocessLine(std::string &line,
                       const std::string &line_leftover) const {
    line = line_leftover + " " + line;
    exclude_comments(line);
    trim_whitespaces(line);
}

/**
 * @brief recursively calls ParseNode() for a sub-node_
 * Parses sub-node_ (child)
 * Assigns child node_ to supernode
 * Saves line leftovers
 * @param current supernode
 * @param line needed to save leftovers
 */
void Config::GetChildNode(RawNode &current,
                          std::ifstream &config,
                          std::string &line) const {
    const RawNode &child = ParseNode(
            config,
            line,
            ParseDirective(line, '{'));
    current.node_.child_nodes_.push_back(child.node_);
    line = child.leftover_;
}

/**
 * @brief Assigns directive to current node_
 */
void Config::GetDirective(std::string &line, RawNode &current,
                          std::ifstream &config) const {
    if (line[0] == ';')
        ThrowSyntaxError("found consecutive semicolons!", config);
    current.node_.directives_.push_back(ParseDirective(line, ';'));
}

Config::Config(const Node &confRoot)
: conf_root_(confRoot) {}

/**
 * @brief Prepares node_ to be returned
 * Removes closing brace
 * Saves leftovers if there are any
 * @throw exception if block is empty, `cause it makes no sense
 */
void Config::FinishSubNode(std::string &line,
                           RawNode &current,
                           std::ifstream &config) const {
    if (current.node_.child_nodes_.empty() && current.node_.directives_.empty())
        ThrowSyntaxError("found an empty block!", config);
    if (current.node_.main_[0] == "main")
        ThrowSyntaxError("found unexpected '}' !", config);
    size_t before_brace = line.find_first_not_of(" \t}");
    size_t brace = line.find_first_of('}');
    if (before_brace < brace)
        ThrowSyntaxError("missing ';' after last directive in the block!",
                         config);
    if (line.size() > 1) {
        line = line.substr(brace + 1);
        current.leftover_ = line;
    } else {
        current.leftover_ = "";
    }
}

/**
 * @brief parses directive into the vector of strings
 *  Here any string that has a ';' character in it, and doesn't have '{'
 * and/or '}' before ';' assumed to be a NGINX directive.
 *  NGINX directives are used to configure various aspects of the NGINX web
 * server. Each directive consists of a keyword and one or more parameters.
 * The parameters are separated from the keyword by spaces or tabs, and each
 * parameter is also separated from the others by spaces or tabs. Directive
 * terminates with ';'
 * @param line to extract directive from
 * @param c terminating character
 * @return vector of strings - parsed directive
 */
v_str Config::ParseDirective(std::string &line, char c) {
    v_str params;
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

/**
 * @brief saves unprocessed line leftovers for next iterations
 * @param line_leftover
 * @param line
 */
void Config::HandleLineLeftower(std::string &line_leftover,
                                std::string &line) const {
    if (!line.empty()) {
        line_leftover = line;
        line = "";
    } else {
        line_leftover = "";
    }
}

void
Config::FinishMainNode(RawNode &current, std::ifstream &config) const {
    if (current.node_.main_[0] != "main")
        ThrowSyntaxError("missing '}' !", config);
}