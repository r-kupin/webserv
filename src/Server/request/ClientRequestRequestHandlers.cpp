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
    if((method_ == POST || method_ == DELETE) && !HasBody())
        ThrowException("POST and DELETE methods should contain body",
                       "BadRequestException");
    if(method_ == GET && HasBody())
        ThrowException("only POST and DELETE methods can have a body",
                       "BadRequestException");
}

std::string ClientRequest::ExtractUrl(const std::string& request) {
    std::string uri = request.substr(request.find_first_of(' ') + 1);
    return (uri.substr(0, uri.find_first_of(' ')));
}

bool ClientRequest::HasBody() {
    return !body_.empty();
}

void ClientRequest::ExtractBody(int max_size) {
    body_ = ExtractBody(max_size, socket_, body_);
}

std::string ClientRequest::ExtractBody(int max_size, int socket,
                                       std::string &body, int buffer_size) {
    if (max_size > 0 && body.size() > (size_t)max_size)
        ThrowException("request body size exceeds limit ",
                       "BodyIsTooLarge");
    char buffer[buffer_size];

    while (true) {
        // int bytes_read = recv(socket, buffer, buffer_size - 1, 0);
        int bytes_read = read(socket, buffer, buffer_size - 1);
        if (bytes_read < 0) {
            throw ReadFromSocketFailedException();
        } else if (bytes_read > 0) {
            // Null-terminate the buffer
            buffer[bytes_read] = '\0';
            // Find the end of the line
            body += std::string(buffer);
            if (max_size > 0 && body.size() > (size_t) max_size)
                ThrowException("request body size exceeds limit ",
                               "BodyIsTooLarge");
        }
        // Nothing (left) to read
        if (bytes_read < buffer_size - 1)
            return body;
    }
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