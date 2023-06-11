/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Location.h                                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/27 14:24:18 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_LOCATION_H
#define WEBSERV_LIB_LOCATION_H

#include <string>
#include <vector>
#include <set>
#include <ostream>
#include "ErrPage.h"
#include "LimitExcept.h"

typedef std::vector<std::string> v_strings;

struct ServerConfiguration;

struct Location {

class LocationException : public std::exception {};
    static const std::map<int, std::string> kHttpOkCodes;
    static const std::map<int, std::string> initializeHttpOkCodes();

    std::set<ErrPage> error_pages_;
    std::set<Location> sublocations_;
    std::set<std::string> index_;
    Limit limit_except_;
    int return_code_;
    int autoindex_;
    std::string return_address_;
    std::string root_;
    std::string address_;

    Location();

    explicit            Location(const std::string &address);

    void                AddErrorPages(const v_strings &directive);
    const Location &    FindSublocationByAddress(const std::string & address) const;
    void                HandleLocationReturn(const v_strings &directives_);
    bool                HasSameAddressAs(const Location &rhs) const;
    bool                HasSameAddressAsOneOfSublocationsOf(const Location &rhs) const;
    void                CheckLocationDirectives(std::vector<v_strings> &directives,
                                 ServerConfiguration &sc);
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_strings &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_strings &directive);
    static void         ThrowLocationError(const std::string &msg);
    void                CheckRoot(const v_strings &root_directive);
    void                HandleRoot(const v_strings &directive);
    std::ostream &      RecursivePrint(std::ostream &os,
                                       const Location &location,
                                       const std::string &prefix) const;

    bool operator<(const Location &rhs) const;
    bool operator==(const Location &rhs) const;
};

struct LocationByAddress {
    std::string targetAddress_;

    explicit LocationByAddress(const std::string& targetAddress);

    bool operator()(const Location& location) const;
};


#endif //WEBSERV_LIB_LOCATION_H
