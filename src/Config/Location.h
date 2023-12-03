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
#include <list>
#include "ErrPage.h"
#include "LimitExcept.h"

class Location;

typedef std::vector<std::string>                v_str;
typedef std::list<std::string>                  l_str;
typedef const std::map<int, std::string>        m_codes_c;
typedef std::list<Location>::iterator           l_loc_it;
typedef std::list<Location>::const_iterator     l_loc_c_it;
typedef std::list<std::string>::const_iterator  l_str_c_it;

class Location {
public:
    class LocationException : public std::exception {};

    static m_codes_c kHttpOkCodes;
    static m_codes_c kHttpRedirectCodes;
    static m_codes_c initializeHttpOKCodes();
    static m_codes_c initializeHttpRedirectCodes();

    std::set<ErrPage>       error_pages_;
    std::list<Location>     sublocations_;
    l_str  index_;
    bool                    index_defined_;
    Limit                   limit_except_;
    int                     return_code_;
    std::string             return_address_;
    std::string             return_custom_message_;
    std::string             root_;
    std::string             address_;
    std::string             full_address_;
    l_loc_it                    parent_;

    Location();
    Location(const Location &);
    explicit Location(const std::string &address);
	Location(const std::string &address, l_loc_it parent);

    void                AddErrorPages(const v_str &directive);
    const Location &    FindSublocationByAddress(const std::string & address) const;
    void                HandleLocationReturn(const v_str &directives_);
    bool                HasSameAddressAs(const Location &rhs) const;
    bool                HasSameAddressAsOneOfSublocationsOf(const Location &rhs) const;
    void                ProcessDirectives(std::vector<v_str> &directives);
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_str &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_str &directive);
    static void         ThrowLocationError(const std::string &msg);
    void                HandleRoot(const v_str &directive);
    std::ostream &      RecursivePrint(std::ostream &os,
                                       const Location &location) const;
    void                UpdateSublocations();
    const Location      &getParent() const;
    bool                operator<(const Location &rhs) const;
    bool                operator==(const Location &rhs) const;
    Location&           operator=(const Location& rhs);
    bool                HasAsSublocation(Location &location);
    void                HandleIndex(const v_str &directives);
    void                HandleCode(const std::string &str);
    void                HandleAddress(const std::string &str);
};

struct LocationByAddress {
    std::string         targetAddress_;
    explicit            LocationByAddress(const std::string& targetAddress);
    bool                operator()(const Location& location) const;
};


#endif //WEBSERV_LIB_LOCATION_H
