/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    main.cpp                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/27 16:06:11 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <cassert>
#include <csignal>
#include "Config/Config.h"
#include "Config/ConfigExceptions.h"

const std::string kDefaultConfig = "resources/nginx_default.conf";

Config try_open_default_config() {
    std::cout << "Trying to open default config..." << std::endl;
    try {
        return Config(std::string(kDefaultConfig));
    } catch (const ConfigFileNotFound& e) {
        std::cout << "Can't open default config file!" << std::endl;
        throw e;
    } catch (const ConfigFileSyntaxError& e) {
        std::cout << "Default config file contains syntax errors!" << std::endl;
        throw e;
    }
}

Config try_open_configs(int ac, char **av) {
    if (ac == 2) {
        try {
            return Config(std::string(av[1]));
        } catch (const ConfigFileNotFound& e) {
            std::cout << "Can't open config file on \"" <<
                    std::string(av[1]) + "\" path!" << std::endl;
            return try_open_default_config();
        } catch (const ConfigFileSyntaxError& e) {
            std::cout << "Config file on \"" + std::string(av[1]) +
                    "\" path! contains syntax errors!" << std::endl;
            return try_open_default_config();
        }
    } else {
        std::cout << "Custom config not provided!" << std::endl;
        return try_open_default_config();
    }
}

int main(int ac, char** av) {
    assert(ac < 3 &&
        "webserv accepts only one argument, and it should be a config file");
    std::cout << "Starting webserv..." << std::endl;
    std::cout <<"Loading config..." << std::endl;
    try {
        Config conf = try_open_configs(ac, av);
    } catch (const Config::ConfigException& e) {
        std::cout << "No config is loaded, startup failed!" << std::endl;
        return (1);
    }
    return 0;
}
