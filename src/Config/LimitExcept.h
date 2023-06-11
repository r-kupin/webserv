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

#ifndef WEBSERV_LIB_LIMITEXCEPT_H
#define WEBSERV_LIB_LIMITEXCEPT_H

#include <string>
#include <vector>
#include <set>
#include <ostream>

enum Methods {GET, POST, DELETE};

struct Limit {
    std::set<Methods> except_;
    int return_code_;
    std::string return_address_;

    Limit();

    bool operator==(const Limit &rhs) const;
};
std::ostream &operator<<(std::ostream &os, const Limit &limit);

#endif //WEBSERV_LIB_LIMITEXCEPT_H
