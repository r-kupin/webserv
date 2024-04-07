/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Host.h                                             :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/04/06 19:07:42 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_HOST_H
#define WEBSERV_LIB_HOST_H

#include <string>

struct Host {
    Host(int port, const std::string &name);
    explicit Host(const std::string &name);
    explicit Host(int port);

    Host        &operator=(const Host &rhs);
    friend bool operator==(const Host &lhs, const Host &rhs);
    bool        operator<(const Host &rhs) const;
    bool        operator>(const Host &rhs) const;
    bool        operator<=(const Host &rhs) const;
    bool        operator>=(const Host &rhs) const;

    int         port_;
    std::string host_;
};


#endif //WEBSERV_LIB_HOST_H
