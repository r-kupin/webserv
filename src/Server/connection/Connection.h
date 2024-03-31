/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Connection.h                                       :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/17 12:34:26 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_CONNECTION_H
#define WEBSERV_CONNECTION_H

#include "request/ClientRequest.h"
#include "../../Config/location/Location.h"

struct Connection {
    Connection(v_c_b &is_running);
    Connection(v_c_b &is_running, int connection_socket, int server_socket);
    Connection(const Connection &other);

    Connection &operator=(const Connection &);

    bool                url_headers_done_;
    bool                body_done_;

    int                 connection_socket_;
    int                 server_listening_socket_;
    ClientRequest       request_;
    Location            location_;
};


#endif //WEBSERV_CONNECTION_H
