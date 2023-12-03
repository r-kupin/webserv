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
        std::cout << "Parsing finished" << std::endl;
        source.close();
        std::cout << "Checking components.. " << std::endl;
        CreateSrvConfigs(conf_root_);
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
    l_srvconf &servers = config.getServers();

    for (l_srvconf_it_c srv = servers.begin();
         srv != servers.end(); ++srv) {
        os << "server: " << std::endl;
        os << "hostname: " << srv->server_name_ << std::endl;
        for (std::set<std::string>::iterator it = srv->server_names_.begin();
             it != srv->server_names_.end(); ++it) {
            os << "name: " << *it << std::endl;
        }
        os << "port: " << srv->port_ << std::endl;
        if (srv->client_max_body_size_) {
            os << "client_max_body_size_: " << srv->client_max_body_size_ <<
               std::endl;
        }
        srv->locations_.begin()->RecursivePrint(os, *srv->locations_.begin());
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

//const std::vector<ServerConfiguration> &Config::getServers() const {
//    return servers_;
//}