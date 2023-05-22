/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Config.cpp                                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 03:25:01 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <fstream>
#include <iostream>
#include <algorithm>
#include "Config.h"
#include "ConfigExceptions.h"


Config::Config() : conf_path_() {}

Config::Config(const Config &other)
: conf_path_(other.conf_path_) {}

/**
 * @brief Creating a config from file
 * 1. Try to open config file. At first step we are looking for the failbit
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
Config::Config(const std::string &config_path)
: conf_path_(config_path) {
    std::ifstream source;
    source.exceptions(std::ifstream::failbit);
    try {
        source.open(conf_path_.c_str());
        source.exceptions(std::ifstream::badbit);
        std::cout << "Opening config on " + conf_path_ << std::endl;
        ParseConfig(source);
        source.close();
        std::cout << "Parsing finished. Checking components.. " << std::endl;
        servers_ = CheckComponents(conf_root_);
        std::cout << "Checking finished. Preconfiguring servers.. " <<
        std::endl;
    } catch (const std::ifstream::failure &e) {
        throw ConfigFileNotFound();
    }
}

Config &Config::operator=(const Config &other) {
    if (this == &other)
        return *this;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Config &config) {
    const std::vector<ServerConfiguration> &servers = config.getServers();
    
    for (size_t i = 0; i < servers.size(); ++i) {
        const ServerConfiguration &srv = servers[i];
        
        os << "server: " << std::endl;
        os << "hostname: " << srv.hostname_ << std::endl;
        for (size_t j = 0; j < srv.server_names_.size(); ++j) {
            os << "name: " << srv.server_names_[j] << std::endl;
        }
        os << "port: " << srv.port_ << std::endl;
        if (srv.client_max_body_size_) {
            os << "client_max_body_size_: " << srv.client_max_body_size_ <<
            std::endl;
        }
        os << servers[i].root_loc_ << std::endl;
        os << std::endl;
    }
    return os;
}

Config::~Config() {}

const char *ConfigFileNotFound::what() const throw() {
    return "Config file not found";
}

const char *ConfigFileSyntaxError::what() const throw() {
    return "Config file contains syntax errors";
}

const std::string &Config::getConfPath() const {
    return conf_path_;
}

const std::vector<ServerConfiguration> &Config::getServers() const {
    return servers_;
}