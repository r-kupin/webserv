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
#include "../server/ServerExceptions.h"
#include "../request/RequestExceptions.h"

int AServer::FillBuffer(char *buffer, int socket, const size_t &size,
                       v_char &storage) const {
    int bytes_read;
    if (storage.empty()) {
//        bytes_read = read(socket, buffer,
//                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size));
        bytes_read = recv(socket, buffer,
                          std::min(static_cast<size_t>(FILE_BUFFER_SIZE), size), MSG_DONTWAIT);
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
                                                                storage.size(), MSG_DONTWAIT);
        bytes_read += storage.size();
        // all data in the buffer now
        storage.clear();
    }
    return bytes_read;
}

bool AServer::FlushBuffer(char *buffer, int file_fd,
                          const std::string &delimiter, int bytes_read) {
    if (bytes_read > 0) {
        // delimiter position marks the end of the file
        size_t  end = Utils::FindInBuffer(buffer, bytes_read, delimiter);
        int     status;
        if (end != std::string::npos) {
            status = write(file_fd, buffer, end);
        } else {
            status = write(file_fd, buffer, bytes_read);
        }
        if (status == -1)
            return false;
    }
    return true;
}

int AServer::PerformUpload(const ClientRequest &request, int socket,
                           int file_fd, const std::string &delimiter,
                           char *buffer, size_t bytes_left, std::ostream &os) {
    (void)os;
    // actual file contents will be stored here
    v_char  storage(request.GetBody());
    int     zeros_count = 0;
    int     bytes_read = 0;

    while(bytes_left > 0) {
        bytes_read = FillBuffer(buffer, socket, bytes_left, storage);

        if (bytes_read < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
//                it's ok, it means that we are processing data faster than curl sends it
//                Log("recv returned -1 and set errno while reading file's contents");
            } else {
                Log("recv failed while reading file's contents");
                return FAILED_IO;
            }
        } else if (bytes_read == 0) {
            zeros_count++;
            if (zeros_count > 100) {
                Log("hundred recvs returned 0 while reading file's contents");
                return BAD_REQUEST;
            }
        } else {
            zeros_count = 0;
            if (!FlushBuffer(buffer, file_fd, delimiter, bytes_read)) {
                Log("unable to write to file");
                return FAILED_IO;
            }
            bytes_left -= bytes_read;
        }
    }
    // file successfully uploaded
    close(file_fd);
    return OK;
}

bool    set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return false;

    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

int AServer::UploadFromCURL(ClientRequest &request, const std::string &filename,
                            int socket, std::ostream &os) {
    // Open the file in append mode
    int                 file_fd = open(filename.c_str(), O_WRONLY);
    const std::string   &content_type = request.GetHeaderValue("Content-Type");
    size_t              bound_pos = content_type.find(kBoundary);

    if (file_fd < 0) {
        Log("Unable to open file to write uploaded data");
        return FAILED_TO_CREATE_OUTPUT_FILE;
    } else if (bound_pos != std::string::npos) {
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
            set_non_blocking(file_fd);
            return PerformUpload(request, socket, file_fd, delimiter, buffer,
                                 bytes_left, os);
        } catch (const SendContinueFailedException & ) {
            return FAILED_IO;
        } catch (const ReadFromSocketFailedException & ) {
            return FAILED_IO; // async io file upload fails here
        } catch (const RequestBodySizeExceedsLimitException & ) {
            return BODY_TOO_LARGE;
        } catch (const BadRequestException & ) {
            return BAD_REQUEST;
        }
    }
    Log("file bound delimitation is missing");
    return BAD_REQUEST;
}