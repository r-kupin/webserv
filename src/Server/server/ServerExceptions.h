/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerExceptions.h                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/27 12:53:35 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_SERVEREXCEPTIONS_H
#define WEBSERV_SERVEREXCEPTIONS_H

#include "Server.h"

class IOFailedException : public Server::ServerException {
public:
    IOFailedException() : Server::ServerException() {}

    const char *what() const throw();
};

class EwouldblockEagainUpload : public Server::ServerException {
public:
    EwouldblockEagainUpload() : Server::ServerException() {}

    const char *what() const throw();
};

class ZeroReadUpload : public Server::ServerException {
public:
    ZeroReadUpload() : Server::ServerException() {}

    const char *what() const throw();
};

#endif //WEBSERV_SERVEREXCEPTIONS_H
