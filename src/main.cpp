/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/03/27 17:07:59 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

// Include necessary headers and declarations for input/output operations, assertions,
// custom exceptions for configuration errors, and the server management logic.
#include <iostream>
#include <cassert>
#include "Config/config/ConfigExceptions.h"
#include "Server/ServerManager.h"

// Attempts to open and return the default server configuration.
// If errors occur, such as the file not found or syntax errors within the file,
// the appropriate exceptions are caught and rethrown after logging to console.
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

// Tries to open a configuration file based on command line arguments.
// If a custom config is provided and it fails to load due to not being found or syntax errors,
// attempts to load the default configuration as a fallback.
// This ensures the server has some configuration to run with, either custom or default.
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

// The main function where execution begins.
// Asserts that the program is called with the correct number of arguments.
// Attempts to load the server configuration using `try_open_configs`.
// If successful, initializes the server with the loaded configuration and starts it.
// If any configuration related exception is caught, reports failure and exits.
int main(int ac, char** av) {
	assert(ac < 3 &&
		"webserv accepts only one argument, and it should be a config file");
	std::cout << "Starting webserv..." << std::endl;
	std::cout <<"Loading config..." << std::endl;
	try {
		Config conf = try_open_configs(ac, av);
		std::cout << "Config is on path " + conf.getConfPath() +
						" is loaded.  Creating servers.." << std::endl;
        ServerManager server_manager;
        server_manager.Init(conf);
        server_manager.Start();
	} catch (const Config::ConfigException &) {
		std::cout << "No config is loaded, startup failed!" << std::endl;
		return (1);
	} catch (const ServerManager::ServerManagerException &) {
        return (1);
    }
	return 0;
}
