/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSubmodules.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/22 16:59:53 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ConfigSubmodules.h"

bool ErrPage::operator==(const ErrPage &rhs) const {
    return address_ == rhs.address_ &&
           code_ == rhs.code_;
}

bool Location::operator==(const Location &rhs) const {
    return address_ == rhs.address_;
}

Location::Location()
        : return_code_(0) {}


bool ServerConfiguration::operator==(const ServerConfiguration &rhs) const {
    return port_ == rhs.port_;
}

ServerConfiguration::ServerConfiguration()
    : port_(8080), client_max_body_size_(0) {}
