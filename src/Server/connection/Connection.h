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
#ifndef WEBSERV_LIB_CONNECTION_H
#define WEBSERV_LIB_CONNECTION_H


#include "request/ClientRequest.h"
#include "../../Config/location/Location.h"

struct Connection {
    Connection();
    Connection(int fd, const ClientRequest &request);
    Connection(const Connection &other);

    Connection &operator=(const Connection &);

    bool            url_headers_done_;
    bool            body_done_;

    int             fd_;
    ClientRequest   request_;
    Location        location_;
};


#endif //WEBSERV_LIB_CONNECTION_H