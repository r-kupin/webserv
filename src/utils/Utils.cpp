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
#include <ctime>
#include <sstream>
#include <sys/stat.h>
#include "Utils.h"

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

std::string Utils::FileToString(const std::string &address) {
    std::ifstream file(address.c_str());

    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
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

std::string Utils::IntToString(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}
