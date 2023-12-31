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

ServerResponse::ServerResponse() {}

ServerResponse::ServerResponse(const ClientRequest &request,
                               const Location &synth,
                               const std::string &server_name, int port)
    : server_name_(server_name), port_(port) {
    (void) request;
    ComposeResponse(synth);
}

void ServerResponse::ComposeResponse(const Location &synth) {
    top_header_ = ComposeTop(synth);
    AddHeader("Server", "WebServ");
    AddHeader("Date", Utils::NiceTimestamp());
    if (synth.return_custom_message_.empty()) {
        if (IsErrorCode(synth.return_code_)) {
            HandleError(synth);
        } else if (IsRedirectCode(synth.return_code_)) {
            HandleRedirect(synth);
        } else {
            std::string body_address;
            if (synth.body_file_.empty()) {
                body_address = synth.root_ + "/" + synth.index_.front();
            } else {
                body_address = synth.body_file_;
            }
            body_str_ = Utils::FileToString(body_address);
        }
    } else {
        body_str_ = synth.return_custom_message_;
    }
    AddHeader("Content-Type", "text/html");
    AddHeader("Content-Length",Utils::IntToString(body_str_.size()));
    AddHeader("Connection", "keep-alive");
}

void ServerResponse::HandleError(const Location &synth) {
    if (synth.HasErrPageForCode(synth.return_code_)) {
        GetDefinedErrorPage(synth);
    } else {
        body_str_ = GeneratePage(synth.return_code_);
    }
}

void ServerResponse::HandleRedirect(const Location &synth) {
    body_str_ = GeneratePage(synth.return_code_);
    if (!synth.return_external_address_.empty()) {
        AddHeader("Location", synth.return_external_address_);
    } else if (!synth.return_internal_address_.empty()) {
        AddHeader("Location",
                  "http://" + server_name_ + ":"+ Utils::IntToString(port_) +
                            synth.return_internal_address_);
    }
}

void ServerResponse::GetDefinedErrorPage(const Location &synth) {
    std::string address = synth.root_ + "/" +
                        synth.FindErrPageForCode(synth.return_code_)->address_;
    if (Utils::CheckFilesystem(address)) {
        body_str_ = Utils::FileToString(address);
    } else {
        body_str_ = GeneratePage(synth.return_code_);
    }
}

std::string ServerResponse::ComposeTop(const Location &location) {
    std::ostringstream oss;

    oss << kHttpVersion << " " << location.return_code_ << " ";
    if (IsErrorCode(location.return_code_)) {
        oss << ErrPage::kHttpErrCodes.find(location.return_code_)->second;
    } else {
        oss << Location::kHttpOkCodes.find(location.return_code_)->second;
    }
    return oss.str();
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

    if (send(dest, response_buffer, response_size, 0) < 0)
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
    os << response.top_header_ << "\n";
    os << "--headers--\n";
    Utils::OutputMap(response.headers_, os);
    os << "--body--\n" << response.body_str_;
    return os;
}

ServerResponse &ServerResponse::operator=(const ServerResponse &other) {
    if (this == &other)
        return *this;
    return *this;
}

ServerResponse::~ServerResponse() {}