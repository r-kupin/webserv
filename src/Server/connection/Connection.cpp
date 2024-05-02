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

// Constructor initializing with running status
Connection::Connection(v_c_b &is_running)
	: url_headers_done_(false),
	body_done_(false),
	open_time_(Utils::Get().TimeNow()),
	connection_socket_(0),
	server_listening_socket_(0),
	request_(is_running),
	method_(""),
	query_string_(""),
	content_type_(""),
	url_("") {}

// Copy constructor
Connection::Connection(const Connection &other)
	: url_headers_done_(other.url_headers_done_),
	body_done_(other.body_done_),
	open_time_(other.open_time_),
	connection_socket_(other.connection_socket_),
	server_listening_socket_(other.server_listening_socket_),
	address_(other.address_),
	request_(other.request_),
	method_(other.method_),
	query_string_(other.query_string_),
	content_type_(other.content_type_),
	url_(other.url_) {}

// Constructor initializing with connection and server socket
Connection::Connection(v_c_b &is_running, int connection_socket, int server_socket)
	: url_headers_done_(false),
	body_done_(false),
	open_time_(Utils::Get().TimeNow()),
	connection_socket_(connection_socket),
	server_listening_socket_(server_socket),
	request_(is_running),
	method_(""),
	query_string_(""),
	content_type_(""),
	url_("") {}

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
	method_ = other.method_;
	query_string_ = other.query_string_;
	content_type_ = other.content_type_;
	url_ = other.url_;
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

// Set and get methods for CGI-related properties
void		Connection::setMethod(const std::string &method) { method_ = method; }
std::string	Connection::getMethod() const { return method_; }

void		Connection::setQueryString(const std::string &query) { query_string_ = query; }
std::string	Connection::getQueryString() const { return query_string_; }

void		Connection::setContentType(const std::string &type) { content_type_ = type; }
std::string	Connection::getContentType() const { return content_type_; }

void		Connection::setUrl(const std::string &url) { url_ = url; }
std::string	Connection::getUrl() const { return url_; }
