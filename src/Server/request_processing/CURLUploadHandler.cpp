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
#include "../server/ServerExceptions.h"
#include "../request/RequestExceptions.h"

int AServer::FillBuffer(char *buffer, int socket, const size_t &size,
                       v_char &storage) const {
    int bytes_read;
    if (storage.empty()) {
//        bytes_read = read(socket, buffer,
//                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size));
        bytes_read = recv(socket, buffer,
                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size), 0);
    } else {
        // it is first iteration after metadata processing, and body_ contains
        // data that was accidentally red. Now we copy it to buffer
        std::copy(storage.begin(), storage.end(), buffer);
        // and read the remaining amount of bytes, in order to fill buffer
        // fully, and not overwrite data we put there before
//        bytes_read = read(socket, buffer + storage.size(),
//                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size) -
//                                                                storage.size());
        bytes_read = recv(socket, buffer + storage.size(),
                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size) -
                                                                storage.size(), 0);
        bytes_read += storage.size();
        // all data in the buffer now
        storage.clear();
    }
    return bytes_read;
}

bool AServer::FlushBuffer(char *buffer, std::ofstream &file,
                         const std::string &delimiter, int bytes_read) {
    if (bytes_read > 0) {
        // delimiter position marks the end of the file
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

int AServer::PerformUpload(const ClientRequest &request, int socket, std::ofstream &file, const std::string &delimiter, char *buffer, size_t bytes_left, std::ostream &os) {
    // actual file contents will be stored here
    v_char      storage(request.GetBody());

    for (int bytes_read = 1; bytes_left > 0;) {
        if (bytes_read == 0) {
            Log("specified body size is bigger then body actually is", os);
            return BAD_REQUEST;
        }
        bytes_read = FillBuffer(buffer, socket, bytes_left, storage);
        if (bytes_read < 0) {
            Log("unable to read from socket", os);
            return FAILED_IO;
        }
        if (!FlushBuffer(buffer, file, delimiter, bytes_read)) {
            Log("unable to write to file", os);
            return FAILED_IO;
        }
        bytes_left -= bytes_read;
    }
    // file successfully uploaded
    file.close();
    return OK;
}

int AServer::UploadFromCURL(ClientRequest &request, const std::string &filename,
                            int socket, std::ostream &os) {
    // Open the file in append mode
    std::ofstream       file(filename.c_str(), std::ios::app);
    const std::string   &content_type = request.GetHeaderValue("Content-Type");
    size_t              bound_pos = content_type.find(kBoundary);

    if (!file.is_open()) {
        Log("Unable to open file to write uploaded data", os);
        return FAILED_TO_CREATE_OUTPUT_FILE;
    }
    if (bound_pos != std::string::npos) {
        char        buffer[FILE_BUFFER_SIZE];
        // get delimiter from "Content-Type" header
        std::string delimiter = content_type.substr(bound_pos + kBoundary.size());
        // and this is how it will appear in actual file:
        delimiter = kHTTPNewline + "--" + delimiter;
        try {
            // If request has "Expect: 100-continue" header - it will wait here
            request.TellClientToContinueIfNeed(socket);
            // Body contains metadata before file contents. Write it to body_
            size_t curl_metadata_size = request.ProcessCURLFileMetadata(socket, delimiter);
            // 2 is added, because the after the ending delimiter "\r\n" is present
            // and it's not accounted into BodySize header value
            size_t bytes_left = (request.GetDeclaredBodySize() + 2) - curl_metadata_size;
            return PerformUpload(request, socket, file, delimiter, buffer,
                                 bytes_left, os);
        } catch (const SendContinueFailedException & ) {
            return FAILED_IO;
        } catch (const ReadFromSocketFailedException & ) {
            return FAILED_IO;
        } catch (const RequestBodySizeExceedsLimitException & ) {
            return BODY_TOO_LARGE;
        } catch (const BadRequestException & ) {
            return BAD_REQUEST;
        }
    }
    Log("file bound delimitation is missing", os);
    return BAD_REQUEST;
}