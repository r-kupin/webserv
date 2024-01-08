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

#include "../../utils/Utils.h"

struct ErrPage {
    std::string address_;
    int code_;

    ErrPage(const std::string &address, int code);
    ErrPage(int code);

    bool operator<(const ErrPage &rhs) const;
    bool operator==(const ErrPage &rhs) const;
};
std::ostream &operator<<(std::ostream &os, const ErrPage &page);

#endif //WEBSERV_LIB_ERRPAGE_H
