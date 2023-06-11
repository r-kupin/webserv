/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ErrPage.cpp                                        :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/03 00:27:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ErrPage.h"


const std::map<int, std::string> ErrPage::initializeHttpErrCodes() {
    std::map<int, std::string> codes;
    codes.insert(std::make_pair(400, "Bad Request"));
    codes.insert(std::make_pair(401, "Unauthorized"));
    codes.insert(std::make_pair(402, "Payment Required"));
    codes.insert(std::make_pair(403, "Forbidden"));
    codes.insert(std::make_pair(404, "Not Found"));
    codes.insert(std::make_pair(405, "Method Not Allowed"));
    codes.insert(std::make_pair(406, "Not Acceptable"));
    codes.insert(std::make_pair(407, "Proxy Authentication Required"));
    codes.insert(std::make_pair(408, "Request Timeout"));
    codes.insert(std::make_pair(409, "Conflict"));
    codes.insert(std::make_pair(410, "Gone"));
    codes.insert(std::make_pair(411, "Length Required"));
    codes.insert(std::make_pair(412, "Precondition Failed"));
    codes.insert(std::make_pair(413, "Payload Too Large"));
    codes.insert(std::make_pair(414, "URI Too Long"));
    codes.insert(std::make_pair(415, "Unsupported Media Type"));
    codes.insert(std::make_pair(416, "Range Not Satisfiable"));
    codes.insert(std::make_pair(417, "Expectation Failed"));
    codes.insert(std::make_pair(418, "I'm a teapot"));
    codes.insert(std::make_pair(421, "Misdirected Request"));
    codes.insert(std::make_pair(422, "Unprocessable Entity"));
    codes.insert(std::make_pair(423, "Locked"));
    codes.insert(std::make_pair(424, "Failed Dependency"));
    codes.insert(std::make_pair(425, "Too Early"));
    codes.insert(std::make_pair(426, "Upgrade Required"));
    codes.insert(std::make_pair(428, "Precondition Required"));
    codes.insert(std::make_pair(429, "Too Many Requests"));
    codes.insert(std::make_pair(431, "Request Header Fields Too Large"));
    codes.insert(std::make_pair(451, "Unavailable For Legal Reasons"));
    codes.insert(std::make_pair(500, "Internal Server Error"));
    codes.insert(std::make_pair(501, "Not Implemented"));
    codes.insert(std::make_pair(502, "Bad Gateway"));
    codes.insert(std::make_pair(503, "Service Unavailable"));
    codes.insert(std::make_pair(504, "Gateway Timeout"));
    codes.insert(std::make_pair(505, "HTTP Version Not Supported"));
    codes.insert(std::make_pair(506, "Variant Also Negotiates"));
    codes.insert(std::make_pair(507, "Insufficient Storage"));
    codes.insert(std::make_pair(508, "Loop Detected"));
    codes.insert(std::make_pair(510, "Not Extended"));
    codes.insert(std::make_pair(511, "Network Authentication Required"));

    return codes;
}

const std::map<int, std::string> ErrPage::kHttpErrCodes =
        ErrPage::initializeHttpErrCodes();


ErrPage::ErrPage(const std::string &address, int code)
: address_(address), code_(code) {}

ErrPage::ErrPage(int code) : code_(code){}

bool ErrPage::operator<(const ErrPage &rhs) const {
    return code_ < rhs.code_;
}

bool ErrPage::operator==(const ErrPage &rhs) const {
    return address_ == rhs.address_ &&
           code_ == rhs.code_;
}

std::ostream& operator<<(std::ostream &os, const ErrPage &page) {
    os << page.code_ << " " << page.address_;
    return  os;
}
