/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    StringUtils.cpp                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/02 13:02:55 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <string>
#include <netdb.h>
#include <cstring>
#include <arpa/inet.h>
#include "Utils.h"

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

std::string Utils::LookupDNS(const std::string &host) {
    struct addrinfo hints, *res;
    std::string ipv4;

    // Set hints for getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Perform DNS lookup
    int status = getaddrinfo(host.c_str(), NULL, &hints, &res);
    if (status == 0) {
        // Convert the first address to string
        struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
        ipv4 = ip;
        // Free the result of getaddrinfo
        freeaddrinfo(res);
    }
    return ipv4;
}

std::string Utils::NbrToString(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

int Utils::FindFirstDifference(const std::string &s1, const std::string &s2) {
    size_t minLength = std::min(s1.length(), s2.length());
    for (size_t i = 0; i < minLength; ++i) {
        if (s1[i] != s2[i]) {
            return i;
        }
    }
    return minLength;
}

size_t Utils::StringToULong(const std::string &str) {
    char* end_ptr;
    const char* val_ptr = str.c_str();
    if (str.find_first_not_of("0123456789") != std::string::npos)
        throw ConversionException();
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

long Utils::StringToNbr(const std::string &str) {
    char* end_ptr;
    const char* val_ptr = str.c_str();
    errno = 0; // To distinguish success/failure after call
    long nbr = strtoul(val_ptr, &end_ptr, 10);
    if ((errno == ERANGE && (nbr == LONG_MAX || nbr == LONG_MIN)) ||
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

v_char Utils::StringToVchar(const std::string &str) {
    v_char vector = v_char(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        vector[i] = str[i];
    }
    return vector;
}

size_t  Utils::FindInBuffer(const char *buffer, size_t buffer_size,
                            const std::string &pattern) {
    for (size_t i = 0; i < buffer_size; ++i) {
        size_t j = 0;
        while (j < pattern.size() && i < buffer_size &&
               buffer[i] == pattern[j]) {
            ++j;
            ++i;
        }
        if (j == pattern.size())
            return i - j;
    }
    return std::string::npos;
}

size_t  Utils::FindInCharVect(const std::vector<char> &buffer,
                               const std::string &pattern) {
    for (size_t i = 0; i < buffer.size(); ++i) {
        size_t j = 0;
        while (j < pattern.size() && i < buffer.size() &&
               buffer[i] == pattern[j]) {
            ++j;
            ++i;
        }
        if (j == pattern.size())
            return i - j;
    }
    return std::string::npos;
}

void Utils::OutputMap(const m_str_str &map, std::ostream &os) {
    for (m_str_str_c_it it = map.begin(); it != map.end(); ++it) {
        os << it->first << " : " << it->second << std::endl;
    }
}

bool Utils::IsPositiveNumber(const std::string &str) {
    return !str.empty() &&
            str.find_first_not_of("1234567890") == std::string::npos;
}

std::string Utils::ExtractMethod(Methods methods) {
    if (methods == GET) {
        return "GET";
    } if (methods == POST) {
        return "POST";
    } if (methods == DELETE) {
        return "DELETE";
    } if (methods == PUT) {
        return "PUT";
    } else {
        return "UNSUPPORTED";
    }
}


