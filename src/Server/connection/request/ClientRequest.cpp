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

ClientRequest::ClientRequest(v_c_b &is_running)
        : is_running_(is_running) {}

ClientRequest& ClientRequest::operator=(const ClientRequest& other) {
    if (this != &other) {
        raw_request_ = other.raw_request_;
        method_ = other.method_;
        addr_ = other.addr_;
        addr_last_step_ = other.addr_last_step_;
        index_request_ = other.index_request_;
        body_ = other.body_;
        fragment_ = other.fragment_;
        params_ = other.params_;
        headers_ = other.headers_;
        socket_ = other.socket_;
        associated_filename_ = other.associated_filename_;
    }
    return *this;
}

void ClientRequest::Init(int client_sock, std::ofstream *log_file) {
    log_file_ = log_file;
    socket_ = client_sock;
    ReadFromSocket(socket_, BUFFER_SIZE);
    CheckRequest(raw_request_);
    std::string url = ExtractUrl(raw_request_[0]);
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
    if (HasHeaders(raw_request_))
        FillHeaders(raw_request_);
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
v_str &ClientRequest::ReadFromSocket(int socket, int buffer_size) {
    char    buffer[buffer_size];
    v_char  storage;

    ProbeSocket(socket, buffer);
    while (is_running_) {
        int bytes_read = recv(socket, buffer, buffer_size - 1,  0);

        if (bytes_read < 1) {
            NothingToRead(bytes_read);
        } else {
            storage.insert(storage.end(), buffer, buffer + bytes_read);

            size_t line_break = Utils::FindInCharVect(storage, kHTTPNewline);

            while (!storage.empty() && line_break != std::string::npos) {
                v_char header_line(storage.begin(), storage.begin() + line_break);
                if (header_line.empty()) {
                    // Done with headers. Start of the body section.
                    body_ = v_char(storage.begin(), storage.end());
                    return raw_request_;
                }
                raw_request_.push_back(std::string(header_line.begin(),
                                                   header_line.end()));
                storage.erase(storage.begin(), storage.begin() + line_break + 2);
                line_break = Utils::FindInCharVect(storage, "\r\n");
            }
        }
    }
    throw Stopped(); // cant use ThrowException() here...
}

void ClientRequest::NothingToRead(int bytes_read) const {
    if (bytes_read == 0) {
        // Client shut down connection
        ThrowException("recv returned 0 while request is incomplete",
                       "ZeroRead");
    } else {
        // All available data was red but it's not enough to finish
        // request processing at this moment. We'll come back later
        // if client would send more data.
        // errno == EAGAIN || errno == EWOULDBLOCK
        ThrowException("recv returned -1 and set errno - No \\r\\n\\r\\n"
                       " after the request's headers section",
                       "EwouldblockEagain");
    }
}

/**
 *  Because multiple events on the same fd might be stacked we are reading
 * them in the infinite loop.
 *  As there is no way to find out how much do we have available - we got to
 * read while recv wouldn't return 0 or -1 and set errno tp EWOULDBLOCK ||
 * EAGAIN.
 *  In this section we simply probe socket - if there is something to read -
 * recv will return 1.
 */
void ClientRequest::ProbeSocket(int socket, char *buffer) const {
    ssize_t probe = recv(socket, buffer, 1, MSG_PEEK);

    if (probe == 0) {
        ThrowException("The probing recv returned 0 - client closed connection",
                       "ZeroRead");
    } else if (probe < 0) {
        // errno == EAGAIN || errno == EWOULDBLOCK
        ThrowException("The probing recv returned -1 with errno set. "
                       "Nothing to read on this socket for now, but connection "
                       "is still open on the client's side.",
                       "EwouldblockEagain");
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

const std::string &ClientRequest::GetAssociatedFilename() const {
    return associated_filename_;
}

void ClientRequest::SetAssociatedFilename(const std::string &associatedFilename) {
    associated_filename_ = associatedFilename;
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
            return Utils::StringToULong(GetHeaderValue("Content-Length"));
        } catch (const Utils::ConversionException &) {
            ThrowException("Content-Length header value is not an unsigned long",
                           "BadRequestException");
        }
    }
    return 0;
}

bool ClientRequest::IsCurlRequest() const {
    return GetHeaderValue("User-Agent").find("curl") != std::string::npos;
}
