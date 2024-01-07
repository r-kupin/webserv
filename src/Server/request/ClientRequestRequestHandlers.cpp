#include <csignal>
#include <iostream>
#include <algorithm>
#include "ClientRequest.h"
#include "RequestExceptions.h"

Methods ClientRequest::ExtractMethod(const std::string &request) {
    if (request.find("POST") != std::string::npos) {
        return POST;
    } else if (request.find("GET") != std::string::npos) {
        return GET;
    } else if (request.find("DELETE") != std::string::npos) {
        return DELETE;
    } else {
        throw UnsupportedClientMethodException();
    }
}

void ClientRequest::CheckRequest(const v_str &request) {
    method_ = ExtractMethod(request[0]);
    if (request[0].find("HTTP/1.1") == std::string::npos)
        ThrowException("HTTP/1.1 is the only supported protocol",
                       "BadRequestException");
    if((method_ == POST || method_ == DELETE) && !HasBody(request))
        ThrowException("POST and DELETE methods should contain body",
                       "BadRequestException");
    if(method_ == GET && HasBody(request))
        ThrowException("only POST and DELETE methods can have a body",
                       "BadRequestException");
}

std::string ClientRequest::ExtractUrl(const std::string& request) {
    std::string uri = request.substr(request.find_first_of(' ') + 1);
    return (uri.substr(0, uri.find_first_of(' ')));
}

bool ClientRequest::HasBody(const v_str &request) {
    v_str_c_it it = std::find(request.begin(), request.end(), "");
    return it != request.end() && ++it != request.end();
}

std::string ClientRequest::ExtractBody(const v_str &request, int max_size) {
    std::string body;

    // find where does body section might start
    v_str_c_it it = std::find(request.begin(), request.end(), "");

    if (it != request.end() && ++it != request.end()) {
        // if it has body - read line by line
        for (int total_size = 0; it != request.end(); ++it) {
            total_size += it->size();
            if (max_size != -1 && total_size > max_size) {
                // client max body size specified and was exceeded by request
                ThrowException("request body size exceeds limit ",
                               "BodyIsTooLarge");
            }
            if (total_size != (int)it->size())
                body += "\r\n";
            body += *it;
        }
    }
    return body;
}

bool ClientRequest::HasHeaders(const v_str &request) {
    return (request.size() > 1 && request[1].find(':') != std::string::npos);
}

void ClientRequest::FillHeaders(const v_str &request) {
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