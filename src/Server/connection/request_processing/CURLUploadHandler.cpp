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
#include <fcntl.h>
#include "../request/RequestExceptions.h"
#include "../../server/AServer.h"
#include "../../server/ServerExceptions.h"

bool    set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

int AServer::UploadFromCURL(ClientRequest &request, const std::string &filename,
                            int socket) {
    int                 file_fd = open(filename.c_str(), O_WRONLY);
    const std::string   &content_type = request.GetHeaderValue("Content-Type");
    // Start of "boundary=" value of "Content-Type" header
    size_t              bound_pos = content_type.find(kBoundary);

    if (file_fd < 0) {
        Log("Unable to open file to write uploaded data");
        return FAILED_TO_CREATE_OUTPUT_FILE;
    } else if (bound_pos != std::string::npos) {
        // get delimiter from "Content-Type" header
        std::string delimiter = content_type.substr(bound_pos + kBoundary.size());
        // and this is how it will appear in actual file:
        delimiter = kHTTPNewline + "--" + delimiter;
        try {
            request.ProcessCURLFileMetadata(socket, delimiter);
            return PerformUpload(request, socket, file_fd, delimiter);
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
    Log("file bound delimitation is missing");
    return BAD_REQUEST;
}

int AServer::PerformUpload(const ClientRequest &request, int socket, int file_fd,
                           const std::string &delimiter) {
    char    buffer[FILE_BUFFER_SIZE];
    v_char  body = request.GetBody();

    // remove metadata
    body.erase(body.begin(), body.begin() + request.GetCurlMetadataLength(delimiter));
    if (!body.empty()) {
        // it is first iteration after metadata processing, and body_ might contain
        // data that was accidentally red. Now we copy it to buffer
        std::copy(body.begin(), body.end(), buffer);
    }
    while (is_running_) {
        ssize_t bytes_read = recv(socket, buffer + body.size(),
                                  static_cast<size_t>(FILE_BUFFER_SIZE) - body.size(),
                                  MSG_DONTWAIT);
        if (bytes_read < 1) {
            if (bytes_read == 0) {
                Log("recv returned 0 while reading file contents.");
                close(file_fd);
                throw ZeroReadUpload();
            } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
                Log("recv returned -1 with EWOULDBLOCK || EAGAIN set while reading "
                    "file contents. We'll try later.");
                close(file_fd);
                throw EwouldblockEagainUpload();
            } else {
                Log("recv returned -1 due to recv failure while reading file contents.");
            }
        } else {
            size_t  end = Utils::FindInBuffer(buffer, bytes_read + body.size(), delimiter);

            if (end != std::string::npos) {
                if (write(file_fd, buffer, end) == -1)
                    Log("Write to file failed");
                else
                    return OK;
            } else {
                if (write(file_fd, buffer, bytes_read + body.size()) == -1)
                    Log("Write to file failed");
                body.clear();
            }
        }
    }
    return OK;
}

//int AServer::PerformUpload(const ClientRequest &request, int socket,
//                           int file_fd, const std::string &delimiter,
//                           char *buffer, size_t curl_meta_size) {
//    size_t  bytes_left = request.GetBytesLeft();
//    v_char  storage(request.GetBody());
//    int     bytes_read = 0;
//
//    while (is_running_ && bytes_left > 0) {
//        bytes_read = FillBuffer(buffer, socket, bytes_left, storage, curl_meta_size);
//
//        if (bytes_read < 0) {
//            if (errno == EWOULDBLOCK || errno == EAGAIN) {
////                it's ok, it means that we are processing data faster than curl sends it
////                Log("recv returned -1 and set errno while reading file's contents");
//            } else {
//                Log("recv failed while reading file's contents");
//                return FAILED_IO;
//            }
//        } else if (bytes_read == 0) {
//            zeros_count++;
//            if (zeros_count > 100) {
//                Log("hundred recvs returned 0 while reading file's contents");
//                return BAD_REQUEST;
//            }
//        } else {
//            zeros_count = 0;
//            if (!FlushBuffer(buffer, file_fd, delimiter, bytes_read)) {
//                Log("unable to write to file");
//                return FAILED_IO;
//            }
//            bytes_left -= bytes_read;
//        }
//    }
//    // file successfully uploaded
//    close(file_fd);
//    return OK;
//}

int AServer::FillBuffer(char *buffer, int socket, const size_t &size, v_char &storage, size_t metadata_size) const {
    int bytes_read;

    if (storage.empty() || storage.size() - metadata_size == 0) {
        // its not a first call
        bytes_read = recv(socket, buffer,
                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size), MSG_DONTWAIT);
    } else {
        int to_take_from_storage = storage.size() - metadata_size;
        // it is first iteration after metadata processing, and body_ contains
        // data that was accidentally red. Now we copy it to buffer
        std::copy(storage.begin() + metadata_size, storage.end(), buffer);
        // and read the remaining amount of bytes, in order to fill buffer
        // fully, and not overwrite data we put there before
//        bytes_read = read(socket, buffer + storage.size(),
//                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size) -
//                                                                storage.size());
        bytes_read = recv(socket, buffer + storage.size(),
                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size) -
                                                                to_take_from_storage, MSG_DONTWAIT);
        bytes_read += to_take_from_storage;
        // all data in the buffer now
        storage.clear();
    }
    return bytes_read;
}

