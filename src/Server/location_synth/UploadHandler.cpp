/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    UploadHandler.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/27 11:45:17 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <algorithm>
#include <csignal>
#include "../ServerExceptions.h"

bool    Server::TryCreateOutputFile(const std::string & dir,
                                 const std::string & filename,
                                 size_t size) const {
    try {
        if (Utils::CheckFilesystem(dir) != DIRECTORY) {
            std::cout << "directory " + dir + " where server is trying to "
                                              "create a file does not exist" << std::endl;
            return false;
        }
        if (Utils::FileExists(filename)) {
            std::cout << "file " + filename + " already exists" << std::endl;
            return false;
        }
        if (!Utils::CheckPermissions(filename)) {
            std::cout << "server doesn't have proper permissions to create " +
                         filename + " file"<< std::endl;
            return false;
        }
        if (!Utils::CheckSpace(filename, size)) {
            std::cout << "not enough disk space to create a file with size " +
                         Utils::NbrToString(size) << std::endl;
            return false;
        }
        return true;
    } catch (const Utils::StatvfsException &) {
        std::cout << "can't check available space with statvfs" << std::endl;
    }
    return false;
}

bool
Server::UploadFile(const ClientRequest &request, l_loc_c_it found, int socket) {
    static int  files_uploaded_;
    std::string dirname;

    if (found->uploads_path_.at(0) == '/')
        dirname = found->uploads_path_;
    else
        dirname = config_.GetConstRoot().root_ + "/" + found->uploads_path_;

    std::string filename(dirname + "/" + Utils::NbrToString(files_uploaded_));
    if (request.HasHeader("User-Agent") && request.HasHeader("Content-Type")) {
        if (TryCreateOutputFile(dirname, filename,
                                request.GetDeclaredBodySize())) {
            if (request.IsCurlRequest()) {
                if (UploadFromCURL(request, filename, socket)) {
                    files_uploaded_++;
                } else {
                    // upload failed
                    return false;
                }
                return true;
            }
            // else: other clients aren't supported yet
            return false;
        } else {
            // throw  503 or something
        }
    } else {
        // required header
    }
    return false;
}
