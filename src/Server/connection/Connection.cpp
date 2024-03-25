/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Connection.cpp                                     :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/17 12:34:26 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
#include "Connection.h"


Connection::Connection(v_c_b &is_running)
    : is_running_(is_running),
    url_headers_done_(false),
    body_done_(false),
    fd_(0),
    request_(is_running) {}

Connection::Connection(const Connection &other)
    : is_running_(other.is_running_),
    url_headers_done_(other.url_headers_done_),
    body_done_(other.body_done_),
    fd_(other.fd_),
    request_(other.request_) {}

Connection &Connection::operator=(const Connection &other) {
    if (this == &other)
        return *this;
    fd_ = other.fd_;
    request_ = other.request_;
    url_headers_done_ = other.url_headers_done_;
    body_done_ = other.body_done_;
    return *this;
}
