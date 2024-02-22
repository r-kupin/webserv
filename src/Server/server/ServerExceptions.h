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

#ifndef WEBSERV_LIB_SERVEREXCEPTIONS_H
#define WEBSERV_LIB_SERVEREXCEPTIONS_H

#include "Server.h"

class AddrinfoCreationFailed : public Server::ServerException {
public:
    AddrinfoCreationFailed() : Server::ServerException() {}

    const char *what() const throw();
};

class SocketOpeningFailureException : public Server::ServerException {
public:
    SocketOpeningFailureException() : Server::ServerException() {}

    const char *what() const throw();
};

class SocketSetOptionsFailureException : public Server::ServerException {
public:
    SocketSetOptionsFailureException() : Server::ServerException() {}

    const char *what() const throw();
};

class SocketBindingFailureException : public Server::ServerException {
public:
    SocketBindingFailureException() : Server::ServerException() {}

    const char *what() const throw();
};

class SocketListeningFailureException : public Server::ServerException {
public:
    SocketListeningFailureException() : Server::ServerException() {}

    const char *what() const throw();
};

class EpollCreationFailed : public Server::ServerException {
public:
    EpollCreationFailed() : Server::ServerException() {}

    const char *what() const throw();
};

class EpollAddFailed : public Server::ServerException {
public:
    EpollAddFailed() : Server::ServerException() {}

    const char *what() const throw();
};

class NotFoundException : public Server::ServerException {
public:
    NotFoundException() : Server::ServerException() {}

    const char *what() const throw();
};




#endif //WEBSERV_LIB_SERVEREXCEPTIONS_H
