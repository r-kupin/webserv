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

ClientRequest::ClientRequest() {}

ClientRequest::ClientRequest(int client_sock) { Init(client_sock);}

void ClientRequest::Init(int client_sock) {
    socket_ = client_sock;
    v_str request = ReadFromSocket(socket_, kBufferSize);
    CheckRequest(request);
    std::string url = ExtractUrl(request[0]);
    CheckURL(url);
    addr_ = ExtractAddr(url);
    addr_last_step_ = ExtractLastAddrStep(addr_);
    if (addr_[addr_.size() - 1] == '/') {
        index_request_ = true;
        if (addr_ != "/")
            addr_ = addr_.substr(0, addr_.size() - 1);
    } else {
        index_request_ = false;
    }
    if (HasFragment(url))
        fragment_ = ExtractFragment(url);
    if (HasQuery(url))
        FillUrlParams(url);
    if (HasHeaders(request))
        FillHeaders(request);
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
v_str ClientRequest::ReadFromSocket(int socket, int buffer_size) {
    char                buffer[buffer_size];
    v_char              storage;
    v_str               request;

    while (true) {
        // int bytes_read = recv(socket, buffer, buffer_size - 1, 0);
        int bytes_read = read(socket, buffer, buffer_size);
        if (bytes_read <= 0)
            ThrowException("unable to read request", "ReadFailed");
        storage.insert(storage.end(), buffer, buffer + bytes_read);

        size_t line_break = Utils::FindInCharVect(storage, kHTTPNewline);

        while (!storage.empty() && line_break != std::string::npos) {
            v_char header_line(storage.begin(), storage.begin() + line_break);
            if (header_line.empty()) {
                // start of body section
                body_ = v_char(storage.begin(), storage.end());
                return request;
            }
            request.push_back(std::string(header_line.begin(),
                                          header_line.end()));
            storage.erase(storage.begin(), storage.begin() + line_break + 2);
            line_break = Utils::FindInCharVect(storage, "\r\n");
        }
        // Request without body
        if (bytes_read < buffer_size - 1)
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
    print_method(os, request.GetMethod());
    os  << request.GetAddress();
    if (request.IsIndexRequest())
        os  << "/";
    if (!request.GetFragment().empty())
        os << " #" << request.GetFragment();
    os << "\n";
    if (!request.GetParams().empty()) {
        os << "--url params--\n";
        Utils::OutputMap(request.GetParams(), os);
    }
    if (!request.GetHeaders().empty()) {
        os << "--headers--\n";
        Utils::OutputMap(request.GetHeaders(), os);
    }
    return os;
}

Methods ClientRequest::GetMethod() const {
    return method_;
}

const std::string &ClientRequest::GetAddress() const {
    return addr_;
}

const std::string &ClientRequest::GetLastStepUri() const {
    return addr_last_step_;
}

const v_char & ClientRequest::GetBody() const {
    return body_;
}

const m_str_str &ClientRequest::GetParams() const {
    return params_;
}

const m_str_str &ClientRequest::GetHeaders() const {
    return headers_;
}

bool ClientRequest::IsIndexRequest() const {
    return index_request_;
}

const std::string &ClientRequest::GetFragment() const {
    return fragment_;
}

void ClientRequest::SetMethod(Methods method) {
    method_ = method;
}

bool        ClientRequest::HasHeaders(const v_str &request) {
    return (request.size() > 1 && request[1].find(':') != std::string::npos);
}

bool ClientRequest::HasHeader(const std::string &key) const {
    m_str_str_c_it header = headers_.find(key);
    return header != headers_.end() && !header->second.empty();
}

std::string ClientRequest::GetHeaderValue(const std::string &key) const {
    if (HasHeader(key)) {
        return headers_.find(key)->second;
    }
    return "";
}

size_t ClientRequest::GetDeclaredBodySize() const {
    if (!HasHeader("Content-Length")) {
        ThrowException("Content-Length header is missing",
                       "BadRequestException");
    } else {
        try {
            return Utils::StringToNbr(GetHeaderValue("Content-Length"));
        } catch (const Utils::ConversionException &) {
            ThrowException("Content-Length header value is not a number",
                           "BadRequestException");
        }
    }
    return 0;
}

bool ClientRequest::IsCurlRequest() const {
    return GetHeaderValue("User-Agent").find("curl") != std::string::npos;
}
