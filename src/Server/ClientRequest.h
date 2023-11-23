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
#include <vector>
#include <map>
#include "../Config/LimitExcept.h"

struct ClientRequest {
    Methods                             method_;
    std::string                         uri_;
    std::map<std::string, std::string>  headers_;

    explicit                    ClientRequest(int client_sock);
};


#endif //WEBSERV_LIB_CLIENTREQUEST_H