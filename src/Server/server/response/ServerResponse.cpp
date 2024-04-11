/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SevrerResponse.cpp                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/10 13:06:09 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <sstream>
#include <iostream>

#include "ServerResponse.h"
#include "../../connection/Connection.h"

ServerResponse::ServerResponse(const Connection &connection)
        :  addr_(connection.address_),
        code_(connection.location_.return_code_) {
    ComposeResponse(connection.location_);
}

std::string ServerResponse::ComposeTop(int return_code) {
    std::ostringstream oss;

    oss << kHttpVersion << " " << return_code << " ";
    oss << Utils::Get().GetCodeDescription(return_code);
    return oss.str();
}

void ServerResponse::ComposeResponse(const Location &synth) {
    top_header_ = ComposeTop(code_);
    if (synth.return_code_ == 100)
        return;
    AddHeader("Server", "WebServ");
    AddHeader("Date", Utils::NiceTimestamp());
    if (synth.return_custom_message_.empty()) {
        if (Utils::Get().IsErrorCode(synth.return_code_)) {
            HandleError(synth);
        } else if (Utils::Get().IsRedirectCode(synth.return_code_)) {
            HandleRedirect(synth);
        } else {
            if (!synth.body_file_.empty()) {
                body_str_ = Utils::FileToString(synth.body_file_);
            } else if (!synth.listing_.empty()) {
                body_str_ = GenerateAutoIndex(synth);
            } else {
                ThrowResponseException(
                        "code is 200 but body_file is empty and custom "
                        "message not defined. This shouldn't happen");
            }
            AddHeader("Connection", "keep-alive");
        }
        AddHeader("Content-Type", "text/html");
    } else {
        AddHeader("Content-Type", "application/octet-stream");
        body_str_ = synth.return_custom_message_;
    }
    AddHeader("Content-Length", Utils::NbrToString(body_str_.size()));
}

void ServerResponse::HandleError(const Location &synth) {
    if (synth.HasErrPageForCode(synth.return_code_)) {
        GetDefinedErrorPage(synth);
    } else {
        body_str_ = GenerateErrorPage(synth.return_code_);
    }
    AddHeader("Connection", "close");
}

void ServerResponse::HandleRedirect(const Location &synth) {
    if (!synth.return_external_address_.empty()) {
        AddHeader("Location", synth.return_external_address_);
    } else if (!synth.return_internal_address_.empty()) {
        AddHeader("Location",
                  "http://" + addr_ + synth.return_internal_address_);
    }
}

void ServerResponse::GetDefinedErrorPage(const Location &synth) {
    std::string address = synth.root_ + "/" +
                        synth.FindErrPageForCode(synth.return_code_)->address_;
    if (Utils::CheckFilesystem(address)) {
        body_str_ = Utils::FileToString(address);
    } else {
        body_str_ = GenerateErrorPage(synth.return_code_);
    }
}

void ServerResponse::SendResponse(int dest) {
    std::stringstream ss;

    ss << top_header_ << "\r\n";
    for (m_str_str_c_it it = headers_.begin(); it != headers_.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n" << body_str_;

    std::string response_string = ss.str();
    const char *response_buffer = response_string.c_str();
    size_t response_size = response_string.size();

    if (send(dest, response_buffer, response_size,  0) < 0)
        ThrowResponseException("send() returned negative number!");
}

void ServerResponse::AddHeader(const std::string &key,
                               const std::string &value) {
    headers_.insert(std::make_pair(key, value));
}

void    ServerResponse::ThrowResponseException(const std::string& msg) {
    std::cout << "Response creation failed: " << msg << std::endl;
    throw ResponseException();
}

std::ostream &operator<<(std::ostream &os, const ServerResponse &response)  {
    os << response.GetTopHeader() << "\n";
    os << "--headers--\n";
    Utils::OutputMap(response.GetHeaders(), os);
//    os << "--body--\n" << response.body_str_;
    return os;
}

ServerResponse &ServerResponse::operator=(const ServerResponse &other) {
    if (this == &other)
        return *this;
    return *this;
}

ServerResponse::~ServerResponse() {}

const std::string &ServerResponse::GetTopHeader() const {
    return top_header_;
}

const m_str_str &ServerResponse::GetHeaders() const {
    return headers_;
}

const std::string &ServerResponse::GetBodyStr() const {
    return body_str_;
}

int ServerResponse::GetCode() const {
    return code_;
}
