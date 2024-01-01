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
    CheckRequest(request);
    std::string url = ExtractUrl(request[0]);
    CheckURL(url);
    addr_ = ExtractAddr(url);
    addr_last_step_ = ExtractLastAddrStep(addr_);
    if (HasFragment(url))
        fragment_ = ExtractFragment(url);
    if (HasQuery(url))
        FillUrlParams(url);
    if (HasHeaders(request))
        FillHeaders(request);
    if (HasBody(request))
        body_ = ExtractBody(request);
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

void    print_method(std::ostream &os, Methods method) {
    switch (method) {
        case GET:
            os << "GET" << " ";
            break;
        case POST:
            os << "POST" << " ";
            break;
        case DELETE:
            os << "DELETE" << " ";
            break;
        default:
            os << "UNSUPPORTED" << " ";
    }
}

std::ostream &operator<<(std::ostream &os, const ClientRequest &request) {
    print_method(os, request.method_);
    os  << request.addr_;
    if (!request.fragment_.empty())
        os << " #" << request.fragment_;
    os << "\n";
    if (!request.params_.empty()) {
        os << "--url params--\n";
        Utils::OutputMap(request.params_, os);
    }
    if (!request.headers_.empty()) {
        os << "--headers--\n";
        Utils::OutputMap(request.headers_, os);
    }
    if (!request.body_.empty())
        os << "--body--\n" << request.body_ << "\n";
    return os;
}

Methods ClientRequest::GetMethod() const {
    return method_;
}

const std::string &ClientRequest::getAddress() const {
    return addr_;
}

const std::string &ClientRequest::getLastStepUri() const {
    return addr_last_step_;
}

const std::string &ClientRequest::getBody() const {
    return body_;
}

const m_str_str &ClientRequest::getParams() const {
    return params_;
}

const m_str_str &ClientRequest::getHeaders() const {
    return headers_;
}

