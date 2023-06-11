/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ErrPage.h                                          :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/27 14:40:03 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_ERRPAGE_H
#define WEBSERV_LIB_ERRPAGE_H

#include <string>
#include <vector>
#include <set>
#include <ostream>
#include <map>

struct ErrPage {
    static const std::map<int, std::string> kHttpErrCodes;

    std::string address_;
    int code_;

    ErrPage(const std::string &address, int code);
    ErrPage(int code);

    bool operator<(const ErrPage &rhs) const;
    bool operator==(const ErrPage &rhs) const;

    static const std::map<int, std::string> initializeHttpErrCodes();
};
std::ostream &operator<<(std::ostream &os, const ErrPage &page);

#endif //WEBSERV_LIB_ERRPAGE_H
