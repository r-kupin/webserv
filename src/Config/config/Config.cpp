/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 03:25:00 by  rokupin          #+#    #+#             */
/*   Updated: 2024/04/08 15:54:11 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// Include necessary headers for file I/O, standard I/O operations, and algorithms.
// Also include the header for custom configuration exceptions.
#include <fstream>
#include <iostream>
#include <algorithm>
#include "ConfigExceptions.h"

// Default constructor initializing the config path to an empty string.
Config::Config() : conf_path_() {}

// Copy constructor for creating a Config object from another Config object,
// copying the configuration path.
Config::Config(const Config &other)
: conf_path_(other.conf_path_) {}

/**
 * @brief Creating a config from file
 * 1. TryCreateOutputFile to open config file. At first step we are looking for the failbit
 *    only, because this bit get's set if we are unable to open the file. If
 *    failed - @throw ConfigFileNotFound
 * 2. Once config successfully opened we perform minimal NGINX syntax check.
 *    To be able to read from file we need to exclude std::ifstream::failbit
 *    from tracked error list, because for some reason - if a file is read
 *    when EOF is encountered, ios_base::failbit will be set together with
 *    ios_base::eofbit, so it will be a false alarm.
 *
 * @param config_path to config
*/

/**
 * Constructor that attempts to open a configuration file, parse it,
 * and perform syntax checking. If the file can't be opened or contains syntax errors,
 * appropriate exceptions are thrown.
 * 
 * @param config_path Path to the configuration file.
 */
Config::Config(const std::string &config_path)
: conf_path_(config_path) {
	std::ifstream source;
	// Configure ifstream to throw an exception if opening the file fails.
	source.exceptions(std::ifstream::failbit);
	try {
		// Attempt to open the file.
		source.open(conf_path_.c_str());
		// Change exceptions to only consider badbit to avoid failbit on EOF.
		source.exceptions(std::ifstream::badbit);
		std::cout << "Opening config on " + conf_path_ << std::endl;
		// Parse the opened config file.
		ParseConfig(source);
		std::cout << "Parsing finished" << std::endl;
		// Close the file after parsing is complete.
		source.close();
		std::cout << "Checking components.. " << std::endl;
		// Additional logic after parsing to further set up server configurations.
		CreateSrvConfigs(conf_root_);
		std::cout << "Checking finished. Preconfiguring servers.. " << std::endl;
	} catch (const std::ifstream::failure &e) {
		// If opening the file fails, throw a ConfigFileNotFound exception.
		throw ConfigFileNotFound();
	}
}

// Methods for throwing syntax errors, closing the file, and displaying an error message
// before throwing a ConfigFileSyntaxError exception.
void Config::ThrowSyntaxError(const std::string &msg, std::ifstream &config) const {
	config.close();
	ThrowSyntaxError(msg);
}
void Config::ThrowSyntaxError(const std::string &msg) const {
	std::cout << "Syntax error: " + msg << std::endl;
	throw ConfigFileSyntaxError();
}

// Assignment operator definition.
Config &Config::operator=(const Config &other) {
	if (this == &other)
		return *this;
	return *this;
}

// Output stream operator for printing server configurations.
std::ostream &operator<<(std::ostream &os, const Config &config) {
	for (l_sc_c_it it = config.servers_.begin(); it != config.servers_.end(); ++it) {
		os << *it << std::endl;
	}
	return os;
}

// Destructor for the Config class.
Config::~Config() {}

// Getter methods for accessing server configurations,
// one returning a constant reference and the other a modifiable reference.
const l_sc &Config::getConstServers() const {
	return servers_;
}
l_sc &Config::GetServers() {
	return servers_;
}

// Exception message methods for not found and syntax error exceptions.
const char *ConfigFileNotFound::what() const throw() {
	return "Config file not found";
}
const char *ConfigFileSyntaxError::what() const throw() {
	return "Config file contains syntax errors";
}

// Getter method for the configuration file path.
const std::string &Config::getConfPath() const {
	return conf_path_;
}
