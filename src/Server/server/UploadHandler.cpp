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

#include "Server.h"

bool        request_has_all_required_headers(const ClientRequest &request) {
    return request.HasHeader("User-Agent") &&
           request.HasHeader("Content-Type") &&
           request.HasHeader("Content-Length");
}

int         Server::UploadFile(ClientRequest &request, l_loc_c_it found,
                               int socket) const {
    std::string dirname = Utils::DirName(found->uploads_path_,
                                         config_.GetConstRoot().root_);
    if (request_has_all_required_headers(request)) {
        if (request.GetAssociatedFilename().empty()) {
            // file supposed to store upload doesn't exist
            request.SetAssociatedFilename(dirname + "/" +
                        Utils::NbrToString(Utils::Get().GetFilesUploaded()));
            if (TryCreateOutputFile(dirname, request.GetAssociatedFilename(),
                                    request.GetDeclaredBodySize())) {
                Utils::Get().IncrementUploadedFiles();
                // file created successfully
                if (request.IsCurlRequest())
                    return UploadFromCURL(request, request.GetAssociatedFilename(), socket);
                // wget doesn't work on nginx - sends file without tailing linebreak
                Log("Only uploads via curl are supported for now");
                return ONLY_CURL_UPLOADS_SUPPORTED;
            }
            return FAILED_TO_CREATE_OUTPUT_FILE;
        } else {
            // continue upload to already existing file
            return UploadFromCURL(request, request.GetAssociatedFilename(), socket);
        }
    }
    Log("Mandatory headers User-Agent, Content-Type and/or Content-Length are "
        "missing");
    return BAD_REQUEST;
}

bool Server::TryCreateOutputFile(const std::string &dir,
                                 const std::string &filename, size_t size) const {
    try {
        if (Utils::CheckFilesystem(dir) != DIRECTORY) {
            Log("directory " + dir + " where server is trying to create a "
                                     "file does not exist");
            return false;
        }
        if (Utils::FileExists(filename)) {
            Log("file " + filename + " already exists");
            return false;
        }
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

