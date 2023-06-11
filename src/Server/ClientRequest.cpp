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

void ClientRequest::ReadFromSocket(int socket) {
    char buffer[BUFFER_SIZE];
    std::string line;


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
            request_.push_back(SanitizeInput(subline));
            line = line.substr(line_break + 2);
        }
        if (bytesRead < BUFFER_SIZE - 1)
            return;
    }
}


ClientRequest::ClientRequest(int client_sock) {
    ReadFromSocket(client_sock) ;
    if (request_[0].find("POST") != std::string::npos) {
        method_ = POST;
    } else if (request_[0].find("GET") != std::string::npos) {
        method_ = GET;
    } else if (request_[0].find("DELETE") != std::string::npos) {
        method_ = DELETE;
    } else {
        throw UnsupportedClientMethodException();
    }
    uri_ = request_[0].substr(request_[0].find_first_of(' ') + 1);
    uri_ = uri_.substr(0, uri_.find_first_of(' '));
    if (request_[0].find("HTTP/1.1") == std::string::npos) {
        throw HTTPVersionNotSupportedException();
    }
}
