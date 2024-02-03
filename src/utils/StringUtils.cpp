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
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <climits>
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

std::string Utils::NbrToString(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
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
