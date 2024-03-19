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
#include "../../server/AServer.h"

bool AServer::TryCreateOutputFile(const std::string &dir, const std::string &filename, size_t size, std::ostream &os) const {
    (void)os;
    try {
        if (Utils::CheckFilesystem(dir) != DIRECTORY) {
            Log("directory " + dir + " where server is trying to create a "
                                     "file does not exist");
            return false;
        }
//        if (Utils::FileExists(filename)) {
//            Log("file " + filename + " already exists");
//            return false;
//        }
        if (!Utils::CheckPermissions(filename)) {
            Log("server doesn't have proper permissions to create " +
                filename + " file");
            return false;
        }
        if (!Utils::CheckSpace(filename, size)) {
            Log("not enough disk space to create a file with size " +
                Utils::NbrToString(size));
            return false;
        }
        return true;
    } catch (const Utils::StatvfsException &) {
        Log("can't check available space with statvfs");
    }
    return false;
}

int AServer::UploadFile(ClientRequest &request, l_loc_c_it found, int socket, std::ostream &os) {
    static int  files_uploaded_;

    std::string dirname;
    if (found->uploads_path_.at(0) == '/')
        dirname = found->uploads_path_;
    else
        dirname = config_.GetConstRoot().root_ + "/" + found->uploads_path_;

    if (request.GetAssociatedFilename().empty()) {
        request.SetAssociatedFilename(dirname + "/" +
                                        Utils::NbrToString(files_uploaded_++));
    }

    if (request.HasHeader("User-Agent") &&
        request.HasHeader("Content-Type") &&
        request.HasHeader("Content-Length")) {
        if (TryCreateOutputFile(dirname, request.GetAssociatedFilename(),
                                request.GetDeclaredBodySize(), os)) {
            // file created successfully
            if (request.IsCurlRequest())
                return UploadFromCURL(request, request.GetAssociatedFilename(), socket);
            // wget doesn't work on nginx - sends file without tailing linebreak
            Log("Only uploads via curl are supported for now");
            return ONLY_CURL_UPLOADS_SUPPORTED;
        }
        return FAILED_TO_CREATE_OUTPUT_FILE;
    }
    Log("Mandatory headers User-Agent, Content-Type and/or Content-Length are "
        "missing");
    return BAD_REQUEST;
}