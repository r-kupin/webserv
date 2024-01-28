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
#include <sstream>
#include <cstring>
#include "../ServerExceptions.h"

bool tell_server_to_start_if_needed(const ClientRequest &request, int socket) {
    if (request.HasHeader("Expect") &&
        request.GetHeaderValue("Expect") == "100-continue") {
        // ensure client that we are ready for the first part of data
        if (!ServerResponse::TellClientToContinue(socket)) {
            // can't send "HTTP/1.1 100 Continue" to given destination
            return false;
        }
    }
    return true;
}

bool    read_up_to_delimiter(const std::string &delimiter, char *buffer,
                             int socket, v_char & storage) {
    while (Utils::FindInCharVect(storage, delimiter) == std::string::npos) {
        int bytes_read = read(socket, buffer, kMetadataBufferSize - 1);
        if (bytes_read <= 0) {
            return false;
        }
        storage.insert(storage.end(), buffer, buffer + bytes_read);
    }
    return true;
}

bool    read_up_to_double_break(const std::string &delimiter, char *buffer,
                                int socket, v_char & storage) {
    while (Utils::FindInCharVect(storage, delimiter) != std::string::npos &&
        Utils::FindInCharVect(storage, kHTTPEndBlock) == std::string::npos) {
        int bytes_read = read(socket, buffer, kMetadataBufferSize - 1);
        if (bytes_read <= 0) {
            return false;
        }
        storage.insert(storage.end(), buffer, buffer + bytes_read);
    }
    return true;
}

bool process_metadata(v_char & storage, int socket, size_t &size,
                      const std::string &delimiter, std::ofstream &file) {
    char    buffer[kFileBufferSize];

    int bytes_read = read(socket, buffer, kMetadataBufferSize - 1);
    if (bytes_read <= 0) {
        // should have body
        return false;
    }
    storage.insert(storage.end(), buffer, buffer + bytes_read);
    if (read_up_to_delimiter(delimiter, buffer, socket, storage) &&
        read_up_to_double_break(delimiter, buffer, socket, storage)) {
        size_t file_start = Utils::FindInCharVect(storage, kHTTPEndBlock) +
                            kHTTPEndBlock.size();
        std::copy(storage.begin() + file_start, storage.end(), buffer);
        file.write(buffer, storage.size() - file_start);
        size -= storage.size();
        return true;
    }
    return false;
}

//bool process_file_metadata(const std::string &delimiter, char *buffer,
//                           int socket, int bytes_read,
//                           size_t &size, std::ofstream &file) {
//    std::stringstream    ss;
//    if (bytes_read <= 0) {
//        // should have body
//        return false;
//    }
//    buffer[bytes_read] = '\0';
//    ss << buffer;
//    if (read_up_to_delimiter(delimiter, buffer, socket, bytes_read, ss) &&
//        read_up_to_double_break(delimiter, buffer, socket, bytes_read, ss)) {
//        size_t file_start = ss.str().find(kHTTPEndBlock) + kHTTPEndBlock.size();
//        const std::string &file_part = ss.str().substr(file_start);
//        size -= ss.str().size();
//        file.write(file_part.c_str(), file_part.size());
//        return true;
//    }
//    return false;
//}

bool copy_file_contents(char *buffer, int socket, std::ofstream &file,
                        const std::string &delimiter, size_t &size) {
    int bytes_read = read(socket, buffer, std::min(kFileBufferSize - 1, size));
    if (bytes_read < 0) {
        return false;
    } else if (bytes_read > 0) {
        size_t end = Utils::FindInBuffer(buffer, bytes_read, delimiter);
        if (end != std::string::npos) {
            file.write(buffer, end);
        } else {
            file.write(buffer, bytes_read);
        }
    }
    size -= bytes_read;
    return true;
}

bool Server::UploadFromCURL(const ClientRequest &request,
                            const std::string &filename, int socket) {
    // Open the file in append mode
    std::ofstream       file(filename.c_str(), std::ios::app);
    const std::string   &content_type = request.GetHeaderValue("Content-Type");
    size_t              bound_pos = content_type.find(kBoundary);
    size_t              size = request.GetDeclaredBodySize();
    if (!file.is_open())
        return false;
    if (bound_pos != std::string::npos) {
        std::string delimiter = kHTTPNewline + "--" +
                            content_type.substr(bound_pos + kBoundary.size());
        char        buffer[kFileBufferSize];
        v_char      storage(request.GetBody());

        if (!tell_server_to_start_if_needed(request, socket)) {
            // can't send "HTTP/1.1 100 Continue" to given destination
            return false;
        }
        if (!process_metadata(storage, socket, size, delimiter, file)) {
            return false;
        }
//        if (request.GetBody().empty()) {
//            int     bytes_read = read(socket, buffer, METADATA_BUFFER_SIZE - 1);
//            if (!process_file_metadata(delimiter, buffer, socket, bytes_read,
//                                       size, file)) {
//                return false;
//            }
//        } else {
//            memcpy(buffer, request.GetBody().c_str(), request.GetBody().size());
//            if (!process_file_metadata(delimiter, buffer, socket,
//                                       request.GetBody().size(), size, file)) {
//                return false;
//            }
//        }
        while (size > 0) {
            if (!copy_file_contents(buffer, socket, file, delimiter, size)) {
                return false;
            }
        }
        // file successfully uploaded
        file.close();
        return true;
    }
    // curl always adds bound file delimitation
    return false;
}
