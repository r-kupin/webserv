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

#ifndef WEBSERV_SERVERCONFIGURATION_H
#define WEBSERV_SERVERCONFIGURATION_H

#include <string>
#include <vector>
#include <set>
#include <ostream>

#include "../location/Location.h"

#define DEFAULT_PORT 4280

class ServerConfiguration;

const static std::string kDefaultResources = "resources";
const static std::string kDefaultConfig = kDefaultResources + "/nginx.conf";

typedef std::list<ServerConfiguration>                  l_sc;
typedef std::list<ServerConfiguration>::const_iterator  l_sc_c_it;

struct Host {
    Host(int port, const std::string &name);
    explicit Host(const std::string &name);
    explicit Host(int port);

    Host &operator=(const Host &rhs);
    friend bool operator==(const Host &lhs, const Host &rhs);

    int         port_;
    std::string host_;
};

typedef std::vector<Host>                               v_hosts;

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
    const s_str             &GetServerNames() const;
    l_loc_it                GetRootIt();
    l_loc_c_it              GetConstRootIt() const;
    const l_loc             &GetLocations() const;
    long                    GetKeepaliveTimeout() const;
    const v_hosts           &GetHosts() const;
    bool HasHost(const std::string &ipv4, int port) const;

    bool                    operator==(const ServerConfiguration &rhs) const;
    ServerConfiguration     &operator=(const ServerConfiguration& rhs);
    friend                  std::ostream &operator<<(std::ostream &os,
                                                    const ServerConfiguration &config);
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
    void                    HandleHost(const v_str &directive);
    void                    HandleServerNames(const v_str &directive);
    void                    HandleKeepaliveTimeout(const v_str &directive);

    bool                    default_host_;

    std::set<int>           ports_;
    v_hosts                 hosts_;
    s_str                   server_names_;
    l_loc                   locations_;
    long                    keepalive_timeout_;
};

typedef ServerConfiguration::LocSearchResult            Srch_Res;
typedef ServerConfiguration::LocConstSearchResult       Srch_c_Res;

#endif //WEBSERV_SERVERCONFIGURATION_H
