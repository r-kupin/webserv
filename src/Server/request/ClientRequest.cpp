/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ClientMessage.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/14 13:45:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <iostream>
#include <algorithm>
#include "ClientRequest.h"
#include "RequestExceptions.h"

const int BUFFER_SIZE = 1024;

ClientRequest::ClientRequest() {}

ClientRequest::ClientRequest(int client_sock) { Init(client_sock);}

const std::string & sanitize_input(const std::string &input) {
    // Implement your input validation and sanitization logic here
    // This can include removing or escaping characters that may cause security issues
    // You may also enforce constraints on input length, format, or content

    // Apply your sanitization rules to the sanitizedInput string

    return input;
}

void ClientRequest::Init(int client_sock) {
    v_str request = ReadFromSocket(client_sock) ;
    std::string uri = ExtractUri(request[0]);
    method_ = ExtractMethod(request[0]);
    if (uri.empty() || (method_ != GET && (HasQuery(uri) || HasFragment(uri))))
        throw BadURI();
    if (HasFragment(uri) || HasQuery(uri))
        addr_ = ExtractAddr(uri);
    else
        addr_ = uri;
    last_step_uri_ = ExtractLastAddrStep(addr_);
    if (request[0].find("HTTP/1.1") == std::string::npos) {
        throw HTTPVersionNotSupportedException();
    }
    if (request.size() > 1)
        FillHeaders(request);
    if (HasFragment(uri))
        fragment_ = uri.substr(uri.find_first_of('#') + 1);
    if (HasQuery(uri))
        FillUriParams(uri);
    if (method_ != GET) {
        if (HasBody(request))
            body_ = request.back();
        else
            throw BadRequestException();
    }
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

void ClientRequest::FillUriParams(const std::string &uri) {
    unsigned long param_separator = uri.find_first_of('?');
    unsigned long fragm_separator = uri.find_first_of('#');
    std::string query;
    if (HasFragment(uri))
        query = uri.substr(param_separator + 1,
                           fragm_separator - param_separator - 1);
    else
        query = uri.substr(param_separator + 1);
    for (size_t separator = query.find_first_of('&');
         separator != std::string::npos;
         separator = query.find_first_of('&')) {
        std::string pair = query.substr(0, separator);
        if (pair.empty())
            return;
        std::string name = pair.substr(0, query.find_first_of('='));
        std::string value = pair.substr(query.find_first_of('=') + 1);
        if (!name.empty() && !value.empty())
            params_.insert(std::make_pair(name, value));
        query = query.substr(pair.size() + 1);
    }
    if (!query.empty()) {
        unsigned long separator = query.find_first_of('=');
        if (separator != std::string::npos &&
            separator != 0 &&
            separator != query.size() - 1) {
            std::string name = query.substr(0, separator);
            std::string value = query.substr(separator + 1);
            if (!name.empty() && !value.empty())
                params_.insert(std::make_pair(name, value));
        }
    }
}

/**
 * The recv() function offers some additional features specific to socket
 * operations, such as the ability to specify flags for advanced socket
 * options and to handle different types of sockets (e.g., TCP or UDP). It
 * provides more flexibility and control over socket communication compared
 * to the read() function.
 * @param socket
 * @return
 */
v_str ClientRequest::ReadFromSocket(int socket) {
    char buffer[BUFFER_SIZE];
    std::string line;
    v_str request;

    while (true) {
//        int bytesRead = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        int bytesRead = read(socket, buffer, BUFFER_SIZE - 1);

        if (bytesRead <= 0)
            throw ReadFromSocketFailedException();

        buffer[bytesRead] = '\0';  // Null-terminate the buffer

        // Find the end of the line
        line += std::string(buffer);

        while (!line.empty()) {
            unsigned long line_break = line.find_first_of("\n\r");
            std::string subline = line.substr(0, line_break);
            request.push_back(sanitize_input(subline));
            line = line.substr(line_break + 2);
        }
        if (bytesRead < BUFFER_SIZE - 1)
            return request;
    }
}

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

std::string ClientRequest::ExtractUri(const std::string& request) {
    std::string uri = request.substr(request.find_first_of(' ') + 1);
    return (uri.substr(0, uri.find_first_of(' ')));
}
std::string ClientRequest::ExtractLastAddrStep(const std::string& uri) {
    unsigned long separator = uri.find_last_of('/');
    if (separator == std::string::npos)
        throw BadURI();
    return (uri.substr(separator + 1));
}

std::string ClientRequest::ExtractAddr(const std::string& uri) {
    unsigned long param_separator = uri.find_first_of('?');
    unsigned long fragm_separator = uri.find_first_of('#');
    if (param_separator == std::string::npos &&
        fragm_separator == std::string::npos) {
        return uri;
    } else {
        if (param_separator == std::string::npos)
            return (uri.substr(0, fragm_separator));
        else if (fragm_separator == std::string::npos)
            return (uri.substr(0, param_separator));
    }
    return (uri.substr(0, param_separator));
}


bool        ClientRequest::HasQuery(const std::string& uri) {
    unsigned long param_separator = uri.find_first_of('?');
    unsigned long fragm_separator = uri.find_first_of('#');

    if (param_separator == std::string::npos &&
        fragm_separator == std::string::npos) {
        return false;
    } else {
        if (param_separator == std::string::npos)
            return false;
        else if (fragm_separator == std::string::npos)
            return param_separator != uri.size() - 1;
        else if (param_separator > fragm_separator)
            throw BadURI(); // fragment should FOLLOW query, not precede it
    }
    return fragm_separator - param_separator > 3;
}

bool ClientRequest::HasFragment(const std::string &uri) {
    unsigned long param_separator = uri.find_first_of('?');
    unsigned long fragm_separator = uri.find_first_of('#');
    if (param_separator == std::string::npos &&
        fragm_separator == std::string::npos) {
        return false;
    } else {
        if (param_separator == std::string::npos)
            return fragm_separator != uri.size() - 1;
        else if (fragm_separator == std::string::npos)
            return false;
        else if (param_separator > fragm_separator)
            throw BadURI(); // fragment should FOLLOW query, not precede it
    }
    return fragm_separator != uri.size() - 1;
}

bool ClientRequest::HasBody(const v_str &request) {
    return *std::find(request.begin(), request.end(), "") !=
           *request.rbegin();
}

Methods ClientRequest::getMethod() const {
    return method_;
}

const std::string &ClientRequest::getAddress() const {
    return addr_;
}

const std::string &ClientRequest::getLastStepUri() const {
    return last_step_uri_;
}

const std::string &ClientRequest::getBody() const {
    return body_;
}

const std::map<std::string, std::string> &ClientRequest::getParams() const {
    return params_;
}

const std::map<std::string, std::string> &ClientRequest::getHeaders() const {
    return headers_;
}

