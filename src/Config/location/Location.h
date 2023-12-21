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

#include <list>
#include <ostream>
#include "ErrPage.h"
#include "LimitExcept.h"

struct Location;
//-------------------static creation / initialization---------------------------

typedef const std::map<int, std::string>                        m_codes_c;
typedef std::set<ErrPage>::const_iterator                       s_err_c_it;
typedef std::list<Location>                                     l_loc;
typedef std::list<Location>::iterator                           l_loc_it;
typedef std::list<Location>::const_iterator                     l_loc_c_it;
typedef std::list<std::string>                                  l_str;
typedef std::list<std::string>::const_iterator                  l_str_c_it;

struct Location {
    class LocationException : public std::exception {};

    static m_codes_c kHttpOkCodes;
    static m_codes_c kHttpRedirectCodes;
    static m_codes_c initializeHttpOKCodes();
    static m_codes_c initializeHttpRedirectCodes();

    std::set<ErrPage>       error_pages_;
    l_loc                   sublocations_;
    l_str                   index_;
    bool                    index_defined_;
    Limit                   limit_except_;
    int                     return_code_;
    std::string             return_address_;
    std::string             return_custom_message_;
    std::string             root_;
    std::string             full_address_;
    std::string             address_;
    l_loc_it                parent_;
    bool                    ghost_;

    Location();
    Location(bool ghost, const std::string &address);
    Location(const Location &);
    Location(const std::string &address, l_loc_it parent);
    explicit Location(const std::string &address);

    static Location     GhostLocation(const std::string &address);
//-------------------satic utils------------------------------------------------
    static bool         MarkDefined(const std::string &key, bool &flag,
                                    const v_str &directive);
    static bool         UMarkDefined(const std::string &key, bool &flag,
                                     const v_str &directive);
    static std::string  HandleAddressInConstructor(const std::string &address);
    static std::string  GetParticularAddress(const std::string &address);
    static std::string  SupressConsecutiveSlashes(const std::string &address);
    static v_str        SplitAddress(const std::string &address);
    static void CheckSublocationsAddress(const std::string &address,
                                         const std::string &parent_address);
//-------------------functional stuff-------------------------------------------
    l_loc_c_it          FindConstSublocationByAddress(const std::string & address) const;
    l_loc_it            FindSublocationByAddress(const std::string &address);
    bool                HasSameAddressAs(const Location &rhs) const;
    bool                HasSameAddressAsOneOfSublocationsOf(const Location &rhs) const;
    bool                HasAsSublocation(const std::string &address) const;
    bool                HasAsSublocation(const Location &location) const;
    bool                HasDefinedLimitExcept() const;
//-------------------setup address----------------------------------------------
    void                HandleAddress(const std::string &str);
//-------------------setup directives handlers----------------------------------
    void                ProcessDirectives(const std::vector<v_str> &directives);
    void                HandleLocationReturn(const v_str &directives_);
    void                Handle1ArgReturn(const v_str &directives_);
    void                Handle2ArgReturn(const v_str &directives_);
    void                HandleIndex(const v_str &directives);
    void                HandleCode(const std::string &str);
    void                HandleRoot(const v_str &directive);
    void                AddErrorPages(const v_str &directive);
//-------------------setup subcontexts handlers---------------------------------
    void                HandleLimitExcept(const Node &node);
    void                UpdateSublocations();
//-------------------operator overloads & exceptions----------------------------
    static void         ThrowLocationException(const std::string &msg);
    bool                operator<(const Location &rhs) const;
    bool                operator==(const Location &rhs) const;
    Location&           operator=(const Location& rhs);
};
std::ostream &operator<<(std::ostream &os, const Location &location);
//-------------------sublocation search predicate class-------------------------
struct LocationByAddress {
    std::string         targetAddress_;

    explicit            LocationByAddress(const std::string& targetAddress);
    bool                operator()(const Location& location) const;
};


#endif //WEBSERV_LIB_LOCATION_H
