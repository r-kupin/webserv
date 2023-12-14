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
/**
        Description

        1. The server starts by reading the nginx configuration file, which
        specifies the port number, server root_ directory, and any other
        settings.
	        1. The program attempts to load the configuration file. The
	        `try_open_configs` function is called to open the custom
	        configuration file if provided, or the default configuration file
	        if not. If there are any errors in opening or reading the
	        configuration file, corresponding error messages are displayed.
	        2. If the configuration file is successfully loaded, a message is
	        printed indicating the path of the loaded configuration file. The
	        configuration object `conf` is created using the loaded
	        configuration file.
	        3. The configuration object `conf` is printed to display its
	        contents.
*/
#include <iostream>
#include <cassert>
#include "Config/config/ConfigExceptions.h"
#include "Server/ServerManager.h"

Config try_open_default_config() {
    std::cout << "Trying to open default config..." << std::endl;
    try {
        return Config(kDefaultConfig);
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
        std::cout << "Config is on path " + conf.getConfPath() +
                    " is loaded.  Creating servers.." << std::endl;
        std::cout << conf << std::endl;
        ServerManager server_manager(conf);
        server_manager.RunAll();
    } catch (const Config::ConfigException& e) {
        std::cout << "No config is loaded, startup failed!" << std::endl;
        return (1);
    }
    return 0;
}
