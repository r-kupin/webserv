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
    return *std::find(request.begin(), request.end(), "") !=
           *request.rbegin();
}

std::string ClientRequest::ExtractBody(const v_str &request) {
    if (HasBody(request))
        return request.back();
    return "";
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