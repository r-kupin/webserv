/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSyntaxChecker.cpp                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/04 00:49:59 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "Config.h"

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
                    ThrowSyntaxError("missing '{'", config);
                } else {
                    --open_braces;
                }
            }
        }
    }
    if (open_braces)
        ThrowSyntaxError("missing '}'", config);
    config.close();
}
