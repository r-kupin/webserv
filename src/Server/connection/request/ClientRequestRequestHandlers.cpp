
#include <iostream>
#include <algorithm>
#include "ClientRequest.h"

Methods     ClientRequest::ExtractMethod(const std::string &request) {
    if (request.find("POST") != std::string::npos) {
        return POST;
    } else if (request.find("GET") != std::string::npos) {
        return GET;
    } else if (request.find("DELETE") != std::string::npos) {
        return DELETE;
    } else {
//        ThrowException("The only supported methods are GET, POST, DELETE",
//                       "UnsupportedClientMethodException");
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
        if (!request[i].empty() && request[i].find(':') != std::string::npos) {
            std::string name = request[i].
                    substr(0,
                           request[i].find_first_of(':'));
            std::string value = request[i].
                    substr(request[i].find_first_of(": ") + 2);
            if (!name.empty() && !value.empty())
                headers_.insert(std::make_pair(name, value));
        }
    }
}