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
#include "ClientRequest.h"
#include "ServerExceptions.h"

const int BUFFER_SIZE = 1024;


const std::string & SanitizeInput(const std::string &input) {
    // Implement your input validation and sanitization logic here
    // This can include removing or escaping characters that may cause security issues
    // You may also enforce constraints on input length, format, or content

    // Apply your sanitization rules to the sanitizedInput string

    return input;
}

v_str read_from_socket(int socket) {
    char buffer[BUFFER_SIZE];
    std::string line;
    v_str request;

    while (true) {
        //  The recv() function offers some additional features specific to
        //  socket operations, such as the ability to specify flags for advanced
        //  socket options and to handle different types of sockets (e.g., TCP or
        //  UDP). It provides more flexibility and control over socket
        //  communication compared to the read() function.
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
            request.push_back(SanitizeInput(subline));
            line = line.substr(line_break + 2);
        }
        if (bytesRead < BUFFER_SIZE - 1)
            return request;
    }
}

Methods get_method(const std::string &request) {
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

std::string get_uri(const std::string& request) {
    std::string uri = request.substr(request.find_first_of(' ') + 1);
    return (uri.substr(0, uri.find_first_of(' ')));
}

std::string get_address(const std::string& uri) {
    unsigned long separator = uri.find_first_of('?');
    if (separator == std::string::npos)
        return uri;
    return (uri.substr(0, separator));
}

std::string get_last_uri_step(const std::string& uri) {
    unsigned long separator = uri.find_last_of('/');
    if (separator == std::string::npos)
        throw BadURI();
    return (uri.substr(separator + 1));
}

bool        uri_has_query(const std::string& uri) {
    return (uri.find_first_of('?') != std::string::npos);
}

bool        request_has_headers(const v_str& request) {
    return request.size() > 1;
}

void ClientRequest::fill_headers(const v_str &request) {
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

void ClientRequest::fill_uri_params(const std::string &uri) {
    std::string query = uri.substr(uri.find_first_of('?') + 1);
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
        std::string name = query.substr(0, query.find_first_of('='));
        std::string value = query.substr(query.find_first_of('=') + 1);
        if (!name.empty() && !value.empty())
            params_.insert(std::make_pair(name, value));
    }
}

ClientRequest::ClientRequest(int client_sock) {
    v_str request = read_from_socket(client_sock) ;
    std::string uri = get_uri(request[0]);
    if (uri_has_query(uri))
        address_ = get_address(uri);
    else
        address_ = uri;
    method_ = get_method(request[0]);
    // todo: last step, not params!
    last_step_uri_ = get_last_uri_step(uri);
    if (request[0].find("HTTP/1.1") == std::string::npos) {
        throw HTTPVersionNotSupportedException();
    }
    if (request_has_headers(request))
        fill_headers(request);
    if (uri_has_query(uri))
        fill_uri_params(uri);
}
