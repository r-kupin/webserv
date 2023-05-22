/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ClientMessage.h                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/14 13:45:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_CLIENTREQUEST_H
#define WEBSERV_LIB_CLIENTREQUEST_H


#include <string>
#include <netinet/in.h>
#include "../Config/ConfigSubmodules.h"

struct ClientRequest {
    Methods     method_;
    std::string uri_;
    std::string host_;
//    bool        keep_alive_;
    std::string request_;
//    sockaddr_in client_addr_;
    explicit ClientRequest(int client_sock);
};


#endif //WEBSERV_LIB_CLIENTREQUEST_H