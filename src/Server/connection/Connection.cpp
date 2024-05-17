/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 12:34:02 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/15 19:03:47 by mede-mas         ###   ########.fr       */
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
  cgi_stdin_fd_(0),
  cgi_stdout_fd_(0),
  active_cgis_(active_cgis),
  cgi_pid_(-1) {}

Connection::Connection(v_c_b &is_running, int connection_socket, int server_socket, int &active_cgis)
: url_headers_done_(false),
  body_done_(false),
  waiting_for_cgi_(false),
  open_time_(Utils::Get().TimeNow()),
  connection_socket_(connection_socket),
  server_listening_socket_(server_socket),
  request_(is_running),
  cgi_stdin_fd_(0),
  cgi_stdout_fd_(0),
  active_cgis_(active_cgis),
  cgi_pid_(-1) {}

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
  location_(other.location_),
  cgi_stdin_fd_(other.cgi_stdin_fd_),
  cgi_stdout_fd_(other.cgi_stdout_fd_),
  active_cgis_(other.active_cgis_),
  cgi_pid_(other.cgi_pid_) {}

// Assignement operator
Connection &Connection::operator=(const Connection &other) {
	if (this == &other)
		return *this;
    url_headers_done_ = other.url_headers_done_;
    body_done_ = other.body_done_;
    waiting_for_cgi_ = other.waiting_for_cgi_;
    open_time_ = other.open_time_;
    connection_socket_ = other.connection_socket_;
    server_listening_socket_ = other.server_listening_socket_;
    address_ = other.address_;
    request_ = other.request_;
    location_ = other.location_;
    cgi_stdin_fd_ = other.cgi_stdin_fd_;
    cgi_stdout_fd_ = other.cgi_stdout_fd_;
    active_cgis_ = other.active_cgis_;
    cgi_input_buffer_ = other.cgi_input_buffer_;
    cgi_output_buffer_ = other.cgi_output_buffer_;
    to_send_buffer_ = other.to_send_buffer_;
    cgi_pid_ = other.cgi_pid_;
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

std::ostream &operator<<(std::ostream &os, const Connection &connection) {
    os << "url_headers_done_: " << connection.url_headers_done_ << "\n" <<
    " body_done_: " << connection.body_done_ << "\n" <<
    " waiting_for_cgi_: " << connection.waiting_for_cgi_ << "\n" <<
    " open_time_: " << connection.open_time_ << "\n" <<
    " connection_socket_: " << connection.connection_socket_ << "\n" <<
    " server_listening_socket_: " << connection.server_listening_socket_ << "\n" <<
    " address_: " << connection.address_ << "\n" <<
    " request_: " << connection.request_ << "\n" <<
//    " location_: " << connection.location_ << "\n" <<
    " cgi_stdin_fd_: " << connection.cgi_stdin_fd_ << "\n" <<
    " cgi_stdout_fd_: " << connection.cgi_stdout_fd_ << "\n" <<
    " active_cgis_: " << connection.active_cgis_ << "\n";
    return os;
}
