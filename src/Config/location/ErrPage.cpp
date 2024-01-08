/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ErrPage.cpp                                        :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/03 00:27:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ErrPage.h"

ErrPage::ErrPage(const std::string &address, int code)
: address_(address), code_(code) {}

ErrPage::ErrPage(int code) : code_(code){}

bool ErrPage::operator<(const ErrPage &rhs) const {
    return code_ < rhs.code_;
}

bool ErrPage::operator==(const ErrPage &rhs) const {
    return address_ == rhs.address_ &&
           code_ == rhs.code_;
}

std::ostream& operator<<(std::ostream &os, const ErrPage &page) {
    os << page.code_ << " " << page.address_;
    return  os;
}
