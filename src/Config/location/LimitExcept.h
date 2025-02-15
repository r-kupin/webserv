/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LimitExcept.h                                      :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/27 14:41:38 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIMITEXCEPT_H
#define WEBSERV_LIMITEXCEPT_H

#include <string>
#include <vector>
#include <set>
#include <ostream>

#include "../Node.h"

struct Limit {
    class LimitExceptException : public std::exception {};

    std::set<Methods>           except_;
    v_str                       allow_;
    v_str                       deny_;
    bool                        deny_all_;
    bool                        allow_all_;

    Limit();

    void    LimExDenyAddress(const std::string &address);
    void    LimExAllowAddress(const std::string &address);
    void    LimExHandleMethods(const v_str &main);
    void    LimExHandleDirectives(const std::vector<v_str> &directives);
    void    ThrowLimitExceptError(const std::string &msg) const;

    bool operator==(const Limit &rhs) const;
    bool operator!=(const Limit &rhs) const;
    friend  std::ostream &operator<<(std::ostream &os, const Limit &limit);
};

#endif //WEBSERV_LIMITEXCEPT_H
