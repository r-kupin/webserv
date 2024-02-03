/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Utils.cpp                                          :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/31 14:45:45 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include "Utils.h"

m_codes Utils::err_codes;
m_codes Utils::ok_codes;
m_codes Utils::redirect_codes;

m_codes Utils::initializeHttpErrCodes() {
    m_codes map;

    map.insert(std::make_pair(400, "Bad Request"));
    map.insert(std::make_pair(401, "Unauthorized"));
    map.insert(std::make_pair(402, "Payment Required"));
    map.insert(std::make_pair(403, "Forbidden"));
    map.insert(std::make_pair(404, "Not Found"));
    map.insert(std::make_pair(405, "Method Not Allowed"));
    map.insert(std::make_pair(406, "Not Acceptable"));
    map.insert(std::make_pair(407, "Proxy Authentication Required"));
    map.insert(std::make_pair(408, "Request Timeout"));
    map.insert(std::make_pair(409, "Conflict"));
    map.insert(std::make_pair(410, "Gone"));
    map.insert(std::make_pair(411, "Length Required"));
    map.insert(std::make_pair(412, "Precondition Failed"));
    map.insert(std::make_pair(413, "Payload Too Large"));
    map.insert(std::make_pair(414, "URI Too Long"));
    map.insert(std::make_pair(415, "Unsupported Media Type"));
    map.insert(std::make_pair(416, "Range Not Satisfiable"));
    map.insert(std::make_pair(417, "Expectation Failed"));
    map.insert(std::make_pair(418, "I'm a teapot"));
    map.insert(std::make_pair(421, "Misdirected Request"));
    map.insert(std::make_pair(422, "Unprocessable Entity"));
    map.insert(std::make_pair(423, "Locked"));
    map.insert(std::make_pair(424, "Failed Dependency"));
    map.insert(std::make_pair(425, "Too Early"));
    map.insert(std::make_pair(426, "Upgrade Required"));
    map.insert(std::make_pair(428, "Precondition Required"));
    map.insert(std::make_pair(429, "Too Many Requests"));
    map.insert(std::make_pair(431, "Request Header Fields Too Large"));
    map.insert(std::make_pair(451, "Unavailable For Legal Reasons"));
    map.insert(std::make_pair(500, "Internal Server Error"));
    map.insert(std::make_pair(501, "Not Implemented"));
    map.insert(std::make_pair(502, "Bad Gateway"));
    map.insert(std::make_pair(503, "Service Unavailable"));
    map.insert(std::make_pair(504, "Gateway Timeout"));
    map.insert(std::make_pair(505, "HTTP Version Not Supported"));
    map.insert(std::make_pair(506, "Variant Also Negotiates"));
    map.insert(std::make_pair(507, "Insufficient Storage"));
    map.insert(std::make_pair(508, "Loop Detected"));
    map.insert(std::make_pair(510, "Not Extended"));
    map.insert(std::make_pair(511, "Network Authentication Required"));
    return map;
}

m_codes Utils::initializeHttpRedirectCodes() {
    m_codes map;

    map.insert(std::make_pair(301, "Moved Permanently"));
    map.insert(std::make_pair(302, "Found"));
    map.insert(std::make_pair(303, "See Other"));
    map.insert(std::make_pair(307, "Temporary Redirect"));
    map.insert(std::make_pair(308, "Permanent Redirect"));
    return map;
}

m_codes Utils::initializeHttpOKCodes() {
    m_codes map;

    map.insert(std::make_pair(100, "Continue"));
    map.insert(std::make_pair(101, "Switching Protocols"));
    map.insert(std::make_pair(200, "OK"));
    map.insert(std::make_pair(201, "Created"));
    map.insert(std::make_pair(202, "Accepted"));
    map.insert(std::make_pair(203, "Non-Authoritative Information"));
    map.insert(std::make_pair(204, "No Content"));
    map.insert(std::make_pair(205, "Reset Content"));
    map.insert(std::make_pair(206, "Partial Content"));
    map.insert(std::make_pair(300, "Multiple Choices"));
    map.insert(std::make_pair(301, "Moved Permanently"));
    map.insert(std::make_pair(302, "Found"));
    map.insert(std::make_pair(303, "See Other"));
    map.insert(std::make_pair(304, "Not Modified"));
    map.insert(std::make_pair(305, "Use Proxy"));
    map.insert(std::make_pair(307, "Temporary Redirect"));
    map.insert(std::make_pair(308, "Permanent Redirect"));
    return map;
}

Utils::Utils() {}

bool Utils::IsErrorCode(int code) {
    if (ok_codes.empty()) {
        err_codes = initializeHttpErrCodes();
        redirect_codes = initializeHttpRedirectCodes();
        ok_codes = initializeHttpOKCodes();
    }
    return err_codes.find(code) != err_codes.end();
}

bool Utils::IsOKCode(int code) {
    if (ok_codes.empty()) {
        err_codes = initializeHttpErrCodes();
        redirect_codes = initializeHttpRedirectCodes();
        ok_codes = initializeHttpOKCodes();
    }
    return ok_codes.find(code) != ok_codes.end();
}

bool Utils::IsRedirectCode(int code) {
    if (ok_codes.empty()) {
        err_codes = initializeHttpErrCodes();
        redirect_codes = initializeHttpRedirectCodes();
        ok_codes = initializeHttpOKCodes();
    }
    return redirect_codes.find(code) != redirect_codes.end();
}

bool Utils::IsValidHTTPCode(int code) {
    if (ok_codes.empty()) {
        err_codes = initializeHttpErrCodes();
        redirect_codes = initializeHttpRedirectCodes();
        ok_codes = initializeHttpOKCodes();
    }
    return IsErrorCode(code) || IsOKCode(code);
}

std::string Utils::GetCodeDescription(int code) {
    if (ok_codes.empty()) {
        err_codes = initializeHttpErrCodes();
        redirect_codes = initializeHttpRedirectCodes();
        ok_codes = initializeHttpOKCodes();
    }
    if (IsErrorCode(code)) {
        return err_codes.find(code)->second;
    } else {
        return ok_codes.find(code)->second;
    }
}
