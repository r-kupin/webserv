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

#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <climits>
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

void Utils::OutputMap(const m_str_str &map, std::ostream &os) {
    for (m_str_str_c_it it = map.begin(); it != map.end(); ++it) {
        os << it->first << " : " << it->second << std::endl;
    }
}

int Utils::CheckFilesystem(const std::string &address) {
    struct stat fileInfo;

    if (stat(address.c_str(), &fileInfo) == 0) {
        if (S_ISREG(fileInfo.st_mode))
            return FILE;
        if (S_ISDIR(fileInfo.st_mode))
            return DIRECTORY;
        return ELSE;
    }
    return NOTHING;
}

bool Utils::CheckSpace(const std::string &address, size_t size) {
    struct statvfs stat;
    if (statvfs(address.c_str(), &stat) != 0) {
        throw StatvfsException();
    }
    size_t availableSpace = stat.f_frsize * stat.f_bavail;
    return (availableSpace > size);
}

bool Utils::FileExists(const std::string &address) {
    return CheckFilesystem(address) != NOTHING;
}

bool Utils::CheckPermissions(const std::string &address) {
    std::ofstream file(address.c_str());
    if (!file.is_open()) {
        return false;
    }
    file.close();
    return true;
}

std::string Utils::FileToString(const std::string &address) {
    std::cout << address << std::endl;
    std::ifstream file(address.c_str());

    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

bool Utils::AppendToFile(const std::string &data, const std::string &address) {
    // Open the file in append mode
    std::ofstream file(address.c_str(), std::ios::app);
    if (!file.is_open())
        return false;
    file << data;
    if (!file)
        return false;
    file.close();
    return true;
}

std::string Utils::NiceTimestamp() {
    time_t timestamp = std::time(NULL);
    struct tm *timeinfo = gmtime(&timestamp);

    if (timeinfo == NULL) {
        return "timestamp conversion failed...";
    }

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return buffer;
}

size_t Utils::StringToNbr(const std::string &str) {
    char* end_ptr;
    const char* val_ptr = str.c_str();
    errno = 0; // To distinguish success/failure after call
    size_t nbr = strtoul(val_ptr, &end_ptr, 10);
    if ((errno == ERANGE && (nbr == ULONG_MAX || nbr == 0)) ||
        (errno != 0 && nbr == 0)) {
        throw ConversionException();
    }
    // Check if the entire string was consumed
    if (end_ptr == val_ptr || *end_ptr != '\0') {
        // Handle invalid input
        throw ConversionException();
    }
    return nbr;
}

std::string Utils::NbrToString(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

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
