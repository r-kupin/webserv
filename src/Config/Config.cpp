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
        
        os << "server " << std::endl;
        for (size_t j = 0; j < srv.server_name_.size(); ++j) {
            os << "\tname: " << srv.server_name_[j] << std::endl;
        }
        os << "\tport: " << srv.port_ << std::endl;
        if (!srv.root_.empty())
            os << "\troot: " << srv.root_ << std::endl;
        if (!srv.index_.empty()) {
            os << "\tindex: ";
            for (size_t j = 0; j < srv.index_.size(); ++j) {
                os << " " << srv.index_[j];
            }
            os << std::endl;
        }
        if (srv.client_max_body_size_) {
            os << "\tclient_max_body_size_: " << srv.client_max_body_size_ <<
            std::endl;
        }
        for (size_t j = 0; j < srv.error_pages_.size(); ++j) {
            const ErrPage &err_page = srv.error_pages_[j];
            os << "\terror page " << std::endl;
            os << "\t\taddress: " << err_page.address_ << std::endl;
            os << "\t\tfor codes: ";
            for (size_t k = 0; k < err_page.code_.size(); ++k) {
                os << err_page.code_[k] << " ";
            }
            os << std::endl;
        }
        for (size_t j = 0; j < srv.locations_.size(); ++j) {
            const Location &loc = srv.locations_[j];
            os << "\tlocation " << std::endl;
            os << "\t\taddress: " << loc.address_ << std::endl;
            if (!loc.index_.empty()) {
                os << "\t\tindex: ";
                for (size_t k = 0; k < loc.index_.size(); ++k) {
                    os << loc.index_[k] << " " ;
                }
                os << std::endl;
            }
            if (loc.return_code_)
                os << "\t\treturn code: " << loc.return_code_ << std::endl;
            if (!loc.return_address_.empty())
                os << "\t\treturn address: " << loc.return_address_ << std::endl;
            if (!loc.root_.empty())
                os << "\t\troot: " << loc.root_ << std::endl;
        }
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