/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    FSUtils.cpp                                        :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/02 13:01:21 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include "Utils.h"

int Utils::CheckFilesystem(const std::string &address) {
    struct stat fileInfo;

    if (stat(address.c_str(), &fileInfo) == 0) {
        if (S_ISREG(fileInfo.st_mode))
            return COMM_FILE;
        if (S_ISDIR(fileInfo.st_mode))
            return DIRECTORY;
        return ELSE;
    }
    return NOTHING;
}

bool Utils::FileExists(const std::string &address) {
    return CheckFilesystem(address) != NOTHING;
}

bool Utils::CheckSpace(const std::string &address, size_t size) {
    struct statvfs stat;
    if (statvfs(address.c_str(), &stat) != 0) {
        throw StatvfsException();
    }
    size_t availableSpace = stat.f_frsize * stat.f_bavail;
    return (availableSpace > size);
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
    std::ifstream file(address.c_str());

    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}