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
#include "../../server/Server.h"
#include "../../server/ServerExceptions.h"

int Server::UploadFromCURL(ClientRequest &request, const std::string &filename,
                            int socket) {
    int                 file_fd = open(filename.c_str(), O_WRONLY | O_APPEND | O_NONBLOCK);
    const std::string   &content_type = request.GetHeaderValue("Content-Type");
    // Start of "boundary=" value of "Content-Type" header
    size_t              bound_pos = content_type.find(kBoundary);

    if (file_fd < 0) {
        Log("Unable to open file to write uploaded data", log_file_);
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
    Log("file bound delimitation is missing", log_file_);
    return BAD_REQUEST;
}

void    prepare_buffer(v_char &body, char *buffer, const ClientRequest &request,
                       const std::string &delimiter) {
        // remove metadata
        body.erase(body.begin(), body.begin() +
                    request.GetCurlMetadataLength(delimiter));
        if (!body.empty()) {
            // it is first iteration after metadata processing, and body_ might contain
            // data that was accidentally red. Now we copy it to buffer
            std::copy(body.begin(), body.end(), buffer);
        }
};

int Server::PerformUpload(const ClientRequest &request, int socket, int file_fd,
                           const std::string &delimiter) {
    char    buffer[FILE_BUFFER_SIZE];
    v_char  body = request.GetBody();

    prepare_buffer(body, buffer, request, delimiter);
    while (is_running_) {
        ssize_t bytes_read = recv(socket, buffer + body.size(),
                                  static_cast<size_t>(FILE_BUFFER_SIZE) - body.size(), 0);
        if (bytes_read < 1) {
            NoUpoladDataAvailable(file_fd, bytes_read);
        } else {
            size_t  end = Utils::FindInBuffer(buffer, bytes_read + body.size(), delimiter);

            if (end != std::string::npos) {
                if (write(file_fd, buffer, end) == -1)
                    Log("Write to file failed", log_file_);
                else
                    return OK;
            } else {
                if (write(file_fd, buffer, bytes_read + body.size()) == -1)
                    Log("Write to file failed", log_file_);
                body.clear();
            }
        }
    }
    return OK;
}

void Server::NoUpoladDataAvailable(int file_fd, ssize_t bytes_read) {
    if (bytes_read == 0) {
        Log("recv returned 0 while reading file contents.", log_file_);
        close(file_fd);
        throw ZeroReadUpload();
    } else {
        // errno == EWOULDBLOCK || errno == EAGAIN
        Log("recv returned -1 with EWOULDBLOCK || EAGAIN set while reading "
            "file contents. We'll try later.", log_file_);
        close(file_fd); // why ?
        throw EwouldblockEagainUpload();
    }
}
