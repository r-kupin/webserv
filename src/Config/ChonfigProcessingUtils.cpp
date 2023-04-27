/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ChonfigProcessingUtils.cpp                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/04 00:50:39 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include "Config.h"
#include "ConfigExceptions.h"

void    Config::ThrowSyntaxError(const std::string &msg) const {
    std::cout << "Syntax error: " + msg << std::endl;
    throw ConfigFileSyntaxError();
}

void    Config::ThrowSyntaxError(const std::string &msg,
                                 std::ifstream &config) const {
    config.close();
    ThrowSyntaxError(msg);
}

void    Config::ExcludeComments(std::string &line) const {
    if (!line.empty()) {
        size_t comment_position = line.find('#');
        if (comment_position != std::string::npos) {
            line = line.substr(0, comment_position);
        }
    }
}

void    Config::TrimWhitespaces(std::string &line) const {
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

bool Config::MarkDefined(const std::string &key, bool &flag,
                         const v_strings &directive) const {
    if (directive[0] == key && directive.size() > 1) {
        flag = true;
        return true;
    }
    return false;
}

bool Config::IsNumber(const std::string& str) const {
    if (str.find_first_not_of("0123456789") == std::string::npos)
        return true;
    return false;
}