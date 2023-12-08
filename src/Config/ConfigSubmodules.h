/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSubmodules.h                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/19 23:46:33 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_CONFIGSUBMODULES_H
#define WEBSERV_LIB_CONFIGSUBMODULES_H

#include <string>
#include <vector>
#include <set>
#include <ostream>
#include "location/Location.h"

/**
 * @brief server pre-configuration
 */
class ServerConfiguration {
public:
//    server properties
    bool default_hostname_;
    int port_;
    std::string port_str_;
    size_t client_max_body_size_;
    std::string server_name_;
    std::set<std::string> server_names_;
//    secondary locations
    std::list<Location> locations_;

    ServerConfiguration();
    ServerConfiguration(const ServerConfiguration &);

    void                UpdateIndex(const v_str &directive);
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_str &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_str &directive);
    void                CheckServerDirectives(std::vector<v_str> &directives);
    static void         ThrowServerConfigError(const std::string &msg);
    void                UpdateHostname(const v_str &directives);
    Location            &GetRoot();
    l_loc_it            GetRootIt();

    bool operator==(const ServerConfiguration &rhs) const;
    ServerConfiguration& operator=(const ServerConfiguration& rhs);
};

#endif //WEBSERV_LIB_CONFIGSUBMODULES_H
