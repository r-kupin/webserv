/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 12:34:02 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/02 17:41:03 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CONNECTION_H
#define WEBSERV_CONNECTION_H

#include <ostream>
#include "request/ClientRequest.h"
#include "../../Config/location/Location.h"

struct Connection {
	// Existing constructors
	Connection(v_c_b &is_running, int &active_cgis);
	Connection(v_c_b &is_running, int connection_socket, int server_socket, int &active_cgis);
	Connection(const Connection &other);

	// Assignement operator
	Connection          &operator=(const Connection &);

	// Methods to check connection state
	bool                IsOpen() const;
	long                HowLongBeingActive(long now) const;

	// Connection status flags
	bool                url_headers_done_;
	bool                body_done_;
	bool                waiting_for_cgi_;

	// Time and socket-related members
	long                open_time_;

    friend std::ostream &operator<<(std::ostream &os, const Connection &connection);

    int                 connection_socket_;
	int                 server_listening_socket_;
	std::string         address_;

	// Existing members
	ClientRequest       request_;
	Location            location_;

    int                 cgi_fd_;
    int                 &active_cgis_;
};

#endif //WEBSERV_CONNECTION_H
