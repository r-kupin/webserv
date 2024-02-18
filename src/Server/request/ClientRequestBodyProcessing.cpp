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
                           "SendContinue");
    }
}

int ClientRequest::ReadBodyPart(int socket, int buffer_size, char *buffer) {
//    int bytes_read = read(socket, buffer, buffer_size - 1);
    int bytes_read = recv(socket, buffer, buffer_size - 1, 0);
    if (bytes_read < 0)
        ThrowException("unable to read request's body", "ReadFailed");
    body_.insert(body_.end(), buffer, buffer + bytes_read);
    if (body_.size() > GetDeclaredBodySize())
        ThrowException("request body size exceeds value, specified in header",
                       "BodyIsTooLarge");
    return bytes_read;
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

void    ClientRequest::ReadCURLFileMetadata(const std::string &delimiter,
                                            char *buffer, int socket) {
    while (Utils::FindInCharVect(body_, delimiter) == std::string::npos) {
        if (ReadBodyPart(socket, METADATA_BUFFER_SIZE, buffer) == 0)
            ThrowException("Curl request's body intended to upload a file "
                           "should start with delimiter, seems like it's "
                           "missing", "BadRequestException");
    }
    while (Utils::FindInCharVect(body_, delimiter) != std::string::npos &&
           Utils::FindInCharVect(body_, kHTTPEndBlock) == std::string::npos) {
        if (ReadBodyPart(socket, METADATA_BUFFER_SIZE, buffer) == 0)
            ThrowException("Curl request's body intended to upload a file "
                           "should start with delimiter, followed by "
                           "\"\\r\\n\\r\\\"", "BadRequestException");
    }
}

size_t ClientRequest::ProcessCURLFileMetadata(int socket,
                                              const std::string &delimiter) {
    char    buffer[FILE_BUFFER_SIZE];

    ReadBodyPart(socket, METADATA_BUFFER_SIZE, buffer);
    ReadCURLFileMetadata(delimiter, buffer, socket);
    size_t file_start = Utils::FindInCharVect(body_, kHTTPEndBlock) +
                        kHTTPEndBlock.size();
    // we don't need that metadata... right?
    body_.erase(body_.begin(), body_.begin() + file_start);
    return file_start;
}