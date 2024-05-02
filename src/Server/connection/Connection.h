/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 12:34:02 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/02 16:06:38 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CONNECTION_H
#define WEBSERV_CONNECTION_H

#include "request/ClientRequest.h"
#include "../../Config/location/Location.h"

struct Connection {
	// Existing constructors
	Connection(v_c_b &is_running);
	Connection(v_c_b &is_running, int connection_socket, int server_socket);
	Connection(const Connection &other);

	// Assignement operator
	Connection          &operator=(const Connection &);

	// Methods to check connection state
	bool                IsOpen() const;
	long                HowLongBeingActive(long now) const;

	// CGI-related method declarations
	void				setMethod(const std::string &method);
	std::string			getMethod() const;

	void				setQueryString(const std::string &query);
	std::string			getQueryString() const;

	void				setContentType(const std::string &type);
	std::string			getContentType() const;

	// Connection status flags
	bool                url_headers_done_;
	bool                body_done_;

	// Time and socket-related members
	long                open_time_;
	int                 connection_socket_;
	int                 server_listening_socket_;
	std::string         address_;

	// Existing members
	ClientRequest       request_;
	Location            location_;

	private:
	// New members for storing CGI-related data
	std::string			method_;
	std::string			query_string_;
	std::string			content_type_;
	std::string			url_;
};

#endif //WEBSERV_CONNECTION_H
