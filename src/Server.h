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


class Server {
public:
    Server();

    Server(const Server &);

    Server &operator=(const Server &);

    ~Server();

private:
};


#endif //WEBSERV_LIB_SERVER_H