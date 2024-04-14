/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ClientRequestRequestHandlers.cpp                   :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/22 16:59:53 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <algorithm>
#include <sstream>
#include "ClientRequest.h"

Methods     ClientRequest::ExtractMethod(const std::string &request) {
    if (request.find("POST") != std::string::npos) {
        return POST;
    } else if (request.find("GET") != std::string::npos) {
        return GET;
    } else if (request.find("DELETE") != std::string::npos) {
        return DELETE;
    } else {
        ThrowException("The only supported methods are GET, POST, DELETE",
                       "UnsupportedClientMethodException");
        return UNSUPPORTED;
    }
}

void        ClientRequest::CheckRequest(const v_str &request) {
    method_ = ExtractMethod(request[0]);
    if (request[0].find("HTTP/1.1") == std::string::npos)
        ThrowException("HTTP/1.1 is the only supported protocol",
                       "HTTPVersionNotSupportedException");
    if (!HasHeader("Host"))
        ThrowException("Mandatory header \"Host\" is missing",
                       "BadRequestException");
}

std::string ClientRequest::ExtractUrl(const std::string& request) {
    std::string uri = request.substr(request.find_first_of(' ') + 1);
    return (uri.substr(0, uri.find_first_of(' ')));
}

void        ClientRequest::FillHeaders(const v_str &request) {
    for (size_t i = 1; i < request.size(); ++i) {
        std::string header, value;
        std::istringstream  iss(request[i]);

        if (!std::getline(iss, header, ':') ||
            !(iss >> std::ws && std::getline(iss, value))) {
            ThrowException("Header without value", "BadRequestException");
        }
        if (headers_.find(header) == headers_.end()) {
            headers_[header] = value;
        } else {
            ThrowException("Header redefinition", "BadRequestException");
        }
    }
}