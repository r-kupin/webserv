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
    std::string                         address_;
    std::map<std::string, std::string>  params_;
    std::map<std::string, std::string>  headers_;

    explicit                    ClientRequest(int client_sock);
private:
    void fill_headers(const std::vector<std::string> &request);

    void fill_uri_params(const std::string &uri);
};


#endif //WEBSERV_LIB_CLIENTREQUEST_H