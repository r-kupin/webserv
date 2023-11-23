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

std::vector<std::string> read_from_socket(int socket) {
    char buffer[BUFFER_SIZE];
    std::string line;
    std::vector<std::string> request;

    while (true) {
        //  The recv() function offers some additional features specific to
        //  socket operations, such as the ability to specify flags for advanced
        //  socket options and to handle different types of sockets (e.g., TCP or
        //  UDP). It provides more flexibility and control over socket
        //  communication compared to the read() function.
        int bytesRead = recv(socket, buffer, BUFFER_SIZE - 1, 0);
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

ClientRequest::ClientRequest(int client_sock) {
    std::vector<std::string> request = read_from_socket(client_sock) ;
    method_ = get_method(request[0]);
    uri_ = get_uri(request[0]);
    if (request[0].find("HTTP/1.1") == std::string::npos) {
        throw HTTPVersionNotSupportedException();
    }
    for (size_t i = 1; i < request.size(); ++i) {
        std::string name = request[i].substr(0, request[i].find_first_of(':'));
        std::string value = request[i].substr(
                                    request[i].find_first_of(": ") + 1);
        if (!name.empty() && !value.empty())
            headers_.insert(std::make_pair(name, value));
    }
}

