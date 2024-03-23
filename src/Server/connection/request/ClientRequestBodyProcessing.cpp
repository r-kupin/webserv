/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ClientRequestBodyProcessing.cpp                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/02 19:33:59 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cerrno>
#include "ClientRequest.h"

void ClientRequest::TellClientToContinueIfNeed(int socket) const {
    if (HasHeader("Expect") &&
        GetHeaderValue("Expect") == "100-continue") {
        // ensure client that we are ready for the first part of data
        std::ostringstream oss;
        const int kContinueCode = 100;

        oss << kHttpVersion << " " << kContinueCode << " ";
        oss << Utils::GetCodeDescription(kContinueCode) << kHTTPEndBlock;

        std::string response = oss.str();
        const char *response_buffer = response.c_str();
        size_t response_size = response.size();

        if (send(socket, response_buffer, response_size, 0) < 0)
            ThrowException("Server has \"Expect\": \"100-continue\" header "
                       "set, but server is unable to send this response",
                       "SendContinueFailed");
    }
}

int ClientRequest::ReadBodyToRequest(int socket) {
//    char    buffer[BUFFER_SIZE];
//
//    TellClientToContinueIfNeed(socket);
//    size_t bytes_left = GetDeclaredBodySize();
//    for (int bytes_read = 1; bytes_left > 0;) {
//        if (bytes_read == 0) {
//            Log("specified body size is bigger then body actually is");
//            return BAD_REQUEST;
//        }
//    }
//    while (true) {
//        if (ReadBodyPart(socket, BUFFER_SIZE, buffer) < BUFFER_SIZE - 1)
//            return;
//    }
    return socket;
}

int ClientRequest::ReadBodyPart(int socket, int buffer_size, char *buffer) {
    int bytes_read = recv(socket, buffer, buffer_size - 1,  0);

    if (bytes_read < 1) {
        if (bytes_read == 0) {
            ThrowException("recv returned 0 while reading curl metadata. "
                           "We'll try later.",
                           "ZeroRead");
        } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
            ThrowException("recv returned -1 with EWOULDBLOCK || EAGAIN set "
                           "while reading curl metadata. We'll try later.",
                           "EwouldblockEagain");
        } else {
            ThrowException("recv returned -1 due to recv failure while reading curl metadata",
                           "ReadFailed");
        }
    } else {
        body_.insert(body_.end(), buffer, buffer + bytes_read);
        if (body_.size() > GetDeclaredBodySize())
            ThrowException("request body size exceeds value, specified in header",
                           "BodyIsTooLarge");
    }
    return bytes_read;
}

void    ClientRequest::ReadCURLFileMetadata(const std::string &delimiter,
                                            char *buffer, int socket) {
    // Metadata section starts with delimiter
    while (Utils::FindInCharVect(body_, delimiter) == std::string::npos) {
        ReadBodyPart(socket, METADATA_BUFFER_SIZE, buffer);
    }
    // Metadata section ends with "\r\n\r\n"
    while (Utils::FindInCharVect(body_, delimiter) != std::string::npos &&
           Utils::FindInCharVect(body_, kHTTPEndBlock) == std::string::npos) {
        ReadBodyPart(socket, METADATA_BUFFER_SIZE, buffer);
    }
}

bool ClientRequest::BodyHasAllCURLMetadata(const std::string &delimiter) const {
    if (!body_.empty()) {
        size_t delimiter_position = Utils::FindInCharVect(body_, delimiter);
        size_t endblock_position = Utils::FindInCharVect(body_, kHTTPEndBlock);
        return delimiter_position != std::string::npos &&
            endblock_position != std::string::npos &&
            delimiter_position < endblock_position;
    }
    return false;
}

size_t ClientRequest::GetCurlMetadataLength(const std::string &delimiter) const {
    if (!BodyHasAllCURLMetadata(delimiter)) return 0;
    return Utils::FindInCharVect(body_, kHTTPEndBlock) + kHTTPEndBlock.size();
}

/**
 * As far as we do not know the size of curl metadata until we red it all and
 * parsed - we read it chunk-by-chunk. This way, we may accidentally read a piece
 * from the file contents itself.
 * Regarding the asynchronous design we can't know are we just started to read
 * or coming back after a pause. This bring us 2 possible scenarios when this
 * function can be invoked:
 *  1. Incomplete or absent metadata in the @body_:
 *      @body_ might already contain some metadata red by accident during header
 *      processing or if input interrupted while we were reading it deliberately.
 *      We should read all (remaining) metadata into body and @return it's size
 *      just in case if we accidentally read a piece from the file contents itself.
 *  2. Incoming input stopped while we were processing the contents of the file:
 *      @body_  might contain a piece of data from the file contents that at this
 *      point is already written into the file.
 *      We should remove it, so it won't be written again
 */
size_t ClientRequest::ProcessCURLFileMetadata(int socket,
                                              const std::string &delimiter) {
    if (!BodyHasAllCURLMetadata(delimiter)) {
        char buffer[FILE_BUFFER_SIZE];
        // If request has "Expect: 100-continue" header - it will wait here
        TellClientToContinueIfNeed(socket);
        ReadBodyPart(socket, METADATA_BUFFER_SIZE, buffer);
        ReadCURLFileMetadata(delimiter, buffer, socket);
        return GetCurlMetadataLength(delimiter);
    } else {
        size_t file_content_start_pos = GetCurlMetadataLength(delimiter);
        // this file contents are already written
        if (body_.size() > file_content_start_pos)
            body_.erase(body_.begin() + file_content_start_pos, body_.end());
        return file_content_start_pos;
    }
}