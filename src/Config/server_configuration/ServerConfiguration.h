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

#ifndef WEBSERV_LIB_SERVERCONFIGURATION_H
#define WEBSERV_LIB_SERVERCONFIGURATION_H

#include <string>
#include <vector>
#include <set>
#include <ostream>
#include "../location/Location.h"

class ServerConfiguration;

typedef std::list<ServerConfiguration>::const_iterator  l_sc_c_it;
typedef std::set<std::string>::const_iterator           s_str_c_it;

class ServerConfiguration {
public:
    bool                    default_hostname_;
    int                     port_;
    std::string             port_str_;
    size_t                  client_max_body_size_;
    std::string             server_name_;
    std::set<std::string>   server_names_;
    std::list<Location>     locations_;

    ServerConfiguration();
    ServerConfiguration(const ServerConfiguration &);
//-------------------satic utils------------------------------------------------
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_str &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_str &directive);
//-------------------setup directives handlers----------------------------------
    void                UpdateIndex(const v_str &directive);
    void                ProcessDirectives(std::vector<v_str> &directives);
    void                UpdateHostname(const v_str &directives);
//-------------------setup subcontexts handlers---------------------------------
    void                HandleLocationContext(const Node &context);
    void                RecurseLocations(const Node &context, l_loc_it parent);
    void                CheckLocationContextIsCorrect(const Node &context);
    void                ApplyLimitExceptContext(const Node &context,
                                                Location &current);
    void                ApplyLocationContext(const Node &context,
                                             l_loc_it parent,
                                             Location &current);
//-------------------operator overloads & exceptions----------------------------
    static void         ThrowServerConfigError(const std::string &msg);
    const Location      &GetConstRoot() const;
    Location            &GetRoot();
    l_loc_it            GetRootIt();

    bool operator==(const ServerConfiguration &rhs) const;
    ServerConfiguration& operator=(const ServerConfiguration& rhs);
};

std::ostream &operator<<(std::ostream &os, const ServerConfiguration &config);

#endif //WEBSERV_LIB_SERVERCONFIGURATION_H
