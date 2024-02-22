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

#include "AServer.h"

class AddrinfoCreationFailed : public AServer::ServerException {
public:
    AddrinfoCreationFailed() : AServer::ServerException() {}

    const char *what() const throw();
};

class SocketOpeningFailureException : public AServer::ServerException {
public:
    SocketOpeningFailureException() : AServer::ServerException() {}

    const char *what() const throw();
};

class SocketSetOptionsFailureException : public AServer::ServerException {
public:
    SocketSetOptionsFailureException() : AServer::ServerException() {}

    const char *what() const throw();
};

class SocketBindingFailureException : public AServer::ServerException {
public:
    SocketBindingFailureException() : AServer::ServerException() {}

    const char *what() const throw();
};

class SocketListeningFailureException : public AServer::ServerException {
public:
    SocketListeningFailureException() : AServer::ServerException() {}

    const char *what() const throw();
};

class EpollCreationFailed : public AServer::ServerException {
public:
    EpollCreationFailed() : AServer::ServerException() {}

    const char *what() const throw();
};

class EpollAddFailed : public AServer::ServerException {
public:
    EpollAddFailed() : AServer::ServerException() {}

    const char *what() const throw();
};

class NotFoundException : public AServer::ServerException {
public:
    NotFoundException() : AServer::ServerException() {}

    const char *what() const throw();
};




#endif //WEBSERV_LIB_SERVEREXCEPTIONS_H
