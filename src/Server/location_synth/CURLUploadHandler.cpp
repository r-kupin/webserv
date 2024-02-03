/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    CURLUploadHandler.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/27 12:34:13 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <algorithm>
#include <csignal>
#include "../ServerExceptions.h"
#include "../request/RequestExceptions.h"

int Server::FillBuffer(char *buffer, int socket, const size_t &size,
                       v_char &storage) const {
    int bytes_read;
    if (storage.empty()) {
        bytes_read = read(socket, buffer, std::min(kFileBufferSize, size));
    } else {
        std::copy(storage.begin(), storage.end(), buffer);
        bytes_read = read(socket, buffer + storage.size(),
                          std::min(kFileBufferSize, size) - storage.size());
        bytes_read += storage.size();
        storage.clear();
    }
    return bytes_read;
}

bool Server::FlushBuffer(char *buffer, std::ofstream &file,
                         const std::string &delimiter, int bytes_read) {
    if (bytes_read > 0) {
        size_t end = Utils::FindInBuffer(buffer, bytes_read, delimiter);
        if (end != std::string::npos) {
            file.write(buffer, end);
        } else {
            file.write(buffer, bytes_read);
        }
        if (!file)
            return false;
    }
    return true;
}

int Server::PerformUpload(const ClientRequest &request, int socket,
                          std::ofstream &file, const std::string &delimiter,
                          char *buffer, size_t bytes_left) {
    // actual file contents will be stored here
    v_char      storage(request.GetBody());

    while (bytes_left > 0) {
        int bytes_read = FillBuffer(buffer, socket, bytes_left, storage);
        if (bytes_read < 0) {
            Log("unable to read from socket");
            // return ?
        }
        if (!FlushBuffer(buffer, file, delimiter, bytes_read)) {
            Log("unable to write to file");
            return WRITE_TO_FILE_FAILED;
        }
        bytes_left -= bytes_read;
    }
    // file successfully uploaded
    file.close();
    return OK;
}

int Server::UploadFromCURL(ClientRequest &request, const std::string &filename,
                           int socket) {
    // Open the file in append mode
    std::ofstream       file(filename.c_str(), std::ios::app);
    const std::string   &content_type = request.GetHeaderValue("Content-Type");
    size_t              bound_pos = content_type.find(kBoundary);

    if (!file.is_open()) {
        Log("Unable to open file to write uploaded data");
        return FAILED_TO_CREATE_OUTPUT_FILE;
    }
    if (bound_pos != std::string::npos) {
        // get delimiter from "Content-Type" header
        std::string delimiter = kHTTPNewline + "--" +
                            content_type.substr(bound_pos + kBoundary.size());
        char        buffer[kFileBufferSize];
        try {
            // If request has "Expect: 100-continue" header - it will wait here
            request.TellClientToContinueIfNeed(socket);
            // Body contains metadata before file contents. Write it to body_
            size_t bytes_left = request.GetDeclaredBodySize() -
                                request.ProcessCURLFileMetadata(socket,delimiter);
            return PerformUpload(request, socket, file, delimiter, buffer,
                                 bytes_left);
        } catch (const SendContinueFailedException & ) {
            return false;
        } catch (const ReadFromSocketFailedException & ) {
            return false;
        } catch (const RequestBodySizeExceedsLimitException & ) {
            return BODY_TOO_LARGE;
        } catch (const BadRequestException & ) {
            return BAD_REQUEST;
        }
    }
    Log("file bound delimitation is missing");
    return BAD_REQUEST;
}