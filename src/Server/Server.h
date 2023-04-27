/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Server.h                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:02:49 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_SERVER_H
#define WEBSERV_LIB_SERVER_H

#include "../Config/Config.h"

class Server {
public:
    Server();
    Server(const Server &);
    explicit Server(const ServerConfiguration &);

    Server &operator=(const Server &);

    ~Server();

    void Start();
private:
    ServerConfiguration config_;
    int socket_;
};


#endif //WEBSERV_LIB_SERVER_H