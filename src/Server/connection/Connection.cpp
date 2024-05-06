/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 12:34:02 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/02 17:39:56 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.h"


Connection::Connection(v_c_b &is_running, int &active_cgis)
: url_headers_done_(false),
    body_done_(false),
    waiting_for_cgi_(false),
    open_time_(Utils::Get().TimeNow()),
    connection_socket_(0),
    server_listening_socket_(0),
    request_(is_running),
    active_cgis_(active_cgis) {}

Connection::Connection(v_c_b &is_running, int connection_socket, int server_socket, int &active_cgis)
: url_headers_done_(false),
    body_done_(false),
    waiting_for_cgi_(false),
    open_time_(Utils::Get().TimeNow()),
    connection_socket_(connection_socket),
    server_listening_socket_(server_socket),
    request_(is_running),
    active_cgis_(active_cgis) {}

// Copy constructor
Connection::Connection(const Connection &other)
: url_headers_done_(other.url_headers_done_),
	body_done_(other.body_done_),
    waiting_for_cgi_(other.waiting_for_cgi_),
	open_time_(other.open_time_),
	connection_socket_(other.connection_socket_),
	server_listening_socket_(other.server_listening_socket_),
	address_(other.address_),
	request_(other.request_),
    active_cgis_(other.active_cgis_){}

// Assignement operator
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
	active_cgis_ = other.active_cgis_;
    waiting_for_cgi_ = other.waiting_for_cgi_;
	return *this;
}

// Check if connection is open
bool	Connection::IsOpen() const {
	return connection_socket_ != 0 && server_listening_socket_ != 0;
}

// Computes the time for how long the connection has been active
long	Connection::HowLongBeingActive(long now) const {
	return now - open_time_;
}
