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

typedef std::vector<std::string>            v_strings;
typedef std::list<Location>::iterator       l_it;
typedef const std::map<int, std::string>    code_map;

class Location {
public:
    class LocationException : public std::exception {};

    static code_map kHttpOkCodes;
    static code_map kHttpRedirectCodes;
    static code_map initializeHttpOKCodes();
    static code_map initializeHttpRedirectCodes();

    std::set<ErrPage>       error_pages_;
    std::list<Location>     sublocations_;
    std::set<std::string>   index_;
    Limit                   limit_except_;
    int                     return_code_;
    int                     autoindex_;
    std::string             return_address_;
    std::string             return_custom_message_;
    std::string             root_;
    std::string             address_;
    l_it                    parent_;

    Location();
    Location(const Location &);
    explicit Location(const std::string &address);
	Location(const std::string &address, l_it parent);

    void                AddErrorPages(const v_strings &directive);
    const Location &    FindSublocationByAddress(const std::string & address) const;
    void                HandleLocationReturn(const v_strings &directives_);
    bool                HasSameAddressAs(const Location &rhs) const;
    bool                HasSameAddressAsOneOfSublocationsOf(const Location &rhs) const;
    void                ProcessDirectives(std::vector<v_strings> &directives);
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_strings &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_strings &directive);
    static void         ThrowLocationError(const std::string &msg);
    void                HandleRoot(const v_strings &directive);
    std::ostream &      RecursivePrint(std::ostream &os,
                                       const Location &location,
                                       const std::string &prefix) const;
    void                UpdeteSublocations();
    const Location      &getParent() const;
    bool                operator<(const Location &rhs) const;
    bool                operator==(const Location &rhs) const;
    Location&           operator=(const Location& rhs);
    bool                HasAsSublocation(Location &location);
    void                HandleIndex(const v_strings &directives);

    void HandleCode(const std::string &str);

    void HandleAddress(const std::string &str);
};

struct LocationByAddress {
    std::string         targetAddress_;
    explicit            LocationByAddress(const std::string& targetAddress);
    bool                operator()(const Location& location) const;
};


#endif //WEBSERV_LIB_LOCATION_H
