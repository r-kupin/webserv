/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 12:34:02 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/15 18:20:38 by mede-mas         ###   ########.fr       */
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
	Connection(v_c_b &is_running,
               int connection_socket,
               int server_socket,
               int &active_cgis);
	Connection(const Connection &other);

	// Assignement operator
	Connection          &operator=(const Connection &);
    friend std::ostream &operator<<(std::ostream &os, const Connection &connection);
	// Methods to check connection state
	bool                IsOpen() const;
	long                HowLongBeingActive(long now) const;

	// Connection status flags
	bool                url_headers_done_;
	bool                body_done_;
	bool                waiting_for_cgi_;

	// Time and socket-related members
	long                open_time_;

	int                 connection_socket_;
	int                 server_listening_socket_;
	std::string         address_;
    int                 event_reported_on_;

	// Existing members
	ClientRequest       request_;
	Location            location_;

	int                 cgi_stdin_fd_;
	int                 cgi_stdout_fd_;
	int                 &active_cgis_;
	v_char              cgi_input_buffer_;
	v_char              cgi_output_buffer_;
    int                 cgi_pid_;
    bool                cgi_is_waiting_for_more_;
    size_t              cgi_remaining_body_length_to_recv_;

    v_char              to_send_buffer_;
};

#endif //WEBSERV_CONNECTION_H
