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
    : url_headers_done_(false),
    body_done_(false),
    open_time_(Utils::Get().TimeNow()),
    connection_socket_(0),
    server_listening_socket_(0),
    request_(is_running) {}

Connection::Connection(const Connection &other)
    : url_headers_done_(other.url_headers_done_),
    body_done_(other.body_done_),
    open_time_(other.open_time_),
    connection_socket_(other.connection_socket_),
    server_listening_socket_(other.server_listening_socket_),
    address_(other.address_),
    request_(other.request_){}

Connection::Connection(v_c_b &is_running, int connection_socket, int server_socket)
   : url_headers_done_(false),
   body_done_(false),
   open_time_(Utils::Get().TimeNow()),
   connection_socket_(connection_socket),
   server_listening_socket_(server_socket),
   request_(is_running) {}

Connection &Connection::operator=(const Connection &other) {
    if (this == &other)
        return *this;
    connection_socket_ = other.connection_socket_;
    server_listening_socket_ = other.server_listening_socket_;
    request_ = other.request_;
    url_headers_done_ = other.url_headers_done_;
    body_done_ = other.body_done_;
    address_ = other.address_;
    open_time_ = other.open_time_;
    return *this;
}

bool Connection::IsOpen() const {
    return connection_socket_ != 0 && server_listening_socket_ != 0;
}

long Connection::HowLongBeingActive(long now) const {
    return now - open_time_;
}
