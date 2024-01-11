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

const static std::string kDefaultResources = "resources";
const static std::string kDefaultConfig = kDefaultResources + "/nginx.conf";

typedef std::list<ServerConfiguration>                  l_sc;
typedef std::list<ServerConfiguration>::const_iterator  l_sc_c_it;

class ServerConfiguration {
public:
    class ServerConfigurationException : public std::exception {};

    ServerConfiguration();
    ServerConfiguration(const ServerConfiguration &);
//-------------------Location search structures---------------------------------
    struct LocSearchResult {
        LocSearchResult(const l_loc_it &location, const std::string &status,
                        const std::string &fullAddress,
                        const std::string &leftowerAddress);

        l_loc_it        location_;
        std::string     status_;
        std::string     full_address_;
        std::string     leftower_address_;
    };

    struct LocConstSearchResult {
        LocConstSearchResult(const l_loc_c_it &location,
                             const std::string &status,
                             const std::string &fullAddress,
                             const std::string &leftowerAddress);

        l_loc_c_it      location_;
        std::string     status_;
        std::string     full_address_;
        std::string     leftower_address_;
    };
//-------------------safe search------------------------------------------------
    LocConstSearchResult    FindConstLocation(const std::string &address) const;
//-------------------setup directives handlers----------------------------------
    void                    ProcessDirectives(std::vector<v_str> &directives);
//-------------------setup subcontexts handlers---------------------------------
    void                    HandleLocationContext(const Node &context);
    void                    CheckLocationContextIsCorrect(const Node &context);
//-------------------Getters & operators----------------------------------------
    const Location          &GetConstRoot() const;
    Location                &GetRoot();
    const std::string       &GetServerName() const;
    l_loc_it                GetRootIt();
    l_loc_c_it              GetConstRootIt() const;
    int                     GetPort() const;
    const l_loc             &GetLocations() const;

    bool                    operator==(const ServerConfiguration &rhs) const;
    ServerConfiguration&    operator=(const ServerConfiguration& rhs);
protected:
//-------------------unsafe search----------------------------------------------
    LocSearchResult         FindLocation(const std::string &address);
    void                    RecurseLocations(const Node &context,
                                             l_loc_it parent,
                                             const Location& maybe_current);
    void                    OverrideLocation(const Node &context, l_loc_it current);
    void                    AddNew(const Node &context, const LocSearchResult &result);
//-------------------exceptions-------------------------------------------------
    static void             ThrowServerConfigError(const std::string &msg);
private:
    static bool             MarkDefined(const std::string &key, bool &flag,
                                    const v_str &directive);
    static bool             UMarkDefined(const std::string &key, bool &flag,
                                     const v_str &directive);
    void                    HandlePort(const v_str &directive);

    int                     port_;
    std::string             server_name_;
    std::list<Location>     locations_;
};

typedef ServerConfiguration::LocSearchResult            Srch_Res;
typedef ServerConfiguration::LocConstSearchResult       Srch_c_Res;

std::ostream &operator<<(std::ostream &os, const ServerConfiguration &config);

#endif //WEBSERV_LIB_SERVERCONFIGURATION_H
