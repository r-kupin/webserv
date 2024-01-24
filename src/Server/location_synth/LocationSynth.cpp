/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSynth.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:31:13 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <algorithm>
#include "../ServerExceptions.h"
#include "../request/RequestExceptions.h"

const static std::string kHTTPEndBlock = "\r\n\r\n";
const static std::string kHTTPNewline = "\r\n";
const static std::string kBoundary = "boundary=";

/**
 * Depending on compliance between what was requested and what is being found
 * creates a synthetic location - a copy of the location that was found, but
 * with altered return code, and redirect-related fields, or with a body file
 * set.
 * @param request
 * @return not-exact copy of a location found
 */
Location Server::SynthesizeHandlingLocation(ClientRequest &request,
                                            int socket) {
    Srch_c_Res res = config_.FindConstLocation(request.GetAddress());
    l_loc_c_it found = res.location_;
    Location synth(*found);

    // server doesn't read body here anymore !!
    if (RequestBodyExceedsLimit(found, request)) {
        std::cout << "client intended to send too large body" << std::endl;
        synth.SetReturnCode(413);
    } else if (AccessForbidden(found, request.GetMethod())) {
        // limit_access rule prohibits request
        std::cout << "access forbidden by rule" << std::endl;
        synth.SetReturnCode(403);
    } else if (found->return_code_ == 0) {
        // return redirection rule isn't set
        if (!found->uploads_path_.empty()) {
            // location is dedicated to handle uploads
            if (request.GetMethod() == POST) {
                bool upload_finished = false;
                // only post method is acknowledged to contain upload payload
                if (UploadFile(request, found, upload_finished, socket)) {
                    // upload saved successfully
                    if (upload_finished) {
                        synth.SetReturnCode(200);
                        synth.return_custom_message_ = "Upload successfull";
                    } else {
                        synth.SetReturnCode(100);
                    }
                } else {
                    std::cout << "failed to create output file" << std::endl;
                    synth.SetReturnCode(503);
                }
            } else {
                std::cout << "only POST method should be used to access "
                             "upload locations" << std::endl;
                synth.SetReturnCode(405);
            }
        } else {
            std::string address = found->root_ + res.leftower_address_;
            int fs_status = Utils::CheckFilesystem(address);
            if (fs_status == ELSE) {
                // something exist on specified address, but it is neither a file nor a directory
                std::cout << address + " is neither a file nor a directory.."
                                       "I don't know what to do with it.."
                          << std::endl;
                synth.SetReturnCode(500);
            } else {
                if (request.IsIndexRequest()) {
                    // request's address part of URI ends with "/"
                    SynthIndex(synth, res, fs_status);
                } else {
                    // request's address part of URI has a filename after the last "/"
                    SynthFile(synth, res, fs_status, request.GetAddress());
                }
            }
        }
    }
    return synth;
}

void Server::SynthFile(Location &synth, const Srch_c_Res &res, int fs_status,
                       const std::string &request_address) const {
    const l_loc_c_it &found = res.location_;
    std::string address = found->root_ + res.leftower_address_;
    // request's address part of URI has an address after last "/" check with
    // leftower-address
    if (fs_status == NOTHING) {
        std::cout << "open() \"" + address + "\" failed" << std::endl;
        synth.SetReturnCode(404);
    } else if (fs_status == DIRECTORY) {
        // redirect to index request
        synth.SetReturnCode(301);
        synth.return_internal_address_ = request_address + "/";
    } else {
        synth.body_file_ = address;
        synth.SetReturnCode(200);
    }
}

bool Server::RequestBodyExceedsLimit(l_loc_c_it found, ClientRequest &request) {
    //    Extracting body here, because we couldn't know the max allowed body
    //    size before we found responsible location. And we don't want to
    //    read the whole body in advance, because it can be a 10G file, and it
    //    will crash the server

    if (request.HasHeader("Content-Length")) {
        try {
            if (found->client_max_body_size_ &&
                found->client_max_body_size_ < request.GetDeclaredBodySize()) {
                // Content-Length header value exceeds limit
                return true;
            }
        } catch (const Utils::ConversionException &) {
            // throw: content length header misconfigured
        }
    } else if (request.GetMethod() == POST || request.GetMethod() == DELETE) {
        // throw: POST and DELETE should have a body and "Content-Length" set
    }
    return false;
}

bool Server::AccessForbidden(l_loc_c_it found, Methods method) const {
    if (found->limit_except_.except_.empty() ||
        found->limit_except_.except_.find(method) !=
            found->limit_except_.except_.end() ||
        (found->limit_except_.except_.find(method) ==
            found->limit_except_.except_.end() &&
        found->limit_except_.allow_all_)) {
        return false;
    }
    return true;
}

bool Server::CanCreateFile(const std::string & dir, const std::string & filename,
                   size_t size) {
    try {
        if (Utils::CheckFilesystem(dir) != DIRECTORY) {
            std::cout << "directory " + dir + " where server is trying to "
                         "create a file does not exist" << std::endl;
            return false;
        }
        if (Utils::FileExists(filename)) {
            std::cout << "file " + filename + " already exists" << std::endl;
            return false;
        }
        if (!Utils::CheckPermissions(filename)) {
            std::cout << "server doesn't have proper permissions to create " +
            filename + " file"<< std::endl;
            return false;
        }
        if (!Utils::CheckSpace(filename, size)) {
            std::cout << "not enough disk space to create a file with size " +
                    Utils::NbrToString(size) << std::endl;
            return false;
        }
        return true;
    } catch (const Utils::StatvfsException &) {
        std::cout << "can't check available space with statvfs" << std::endl;
    }
    return false;
}

bool
Server::UploadFile(const ClientRequest &request, l_loc_c_it found, bool &done,
                   int socket) {
    static int  files_uploaded_;
    std::string dirname;

    if (found->uploads_path_.at(0) == '/')
        dirname = found->uploads_path_;
    else
        dirname = config_.GetConstRoot().root_ + "/" + found->uploads_path_;

    std::string filename(dirname + "/" + Utils::NbrToString(files_uploaded_));
    if (request.HasHeader("User-Agent")) {
        if (CanCreateFile(dirname, filename, request.GetDeclaredBodySize())) {
            if (request.IsCurlRequest()) {
                if (UploadFromCURL(request, filename, done, socket)) {
                    if (done)
                        files_uploaded_++;
                } else {
                    // upload failed
                    return false;
                }
                return true;
            }
            // else: other clients
        } else {
            // throw  503 or something
        }
    } else {
        // required header
    }
    return false;
}

bool Server::UploadFromCURL(const ClientRequest &request,
                            const std::string &filename, bool &done,
                            int socket) {
    size_t bound_pos = request.GetHeaderValue("Content-Type").find(kBoundary);

    if (request.HasHeader("Expect") &&
        request.GetHeaderValue("Expect") == "100-continue") {
        // Body is too big to be sent in one chunk, so we need to read it by parts
        // Keep track of size to be sure that it's not exceeding limit
        size_t size = request.GetDeclaredBodySize();
        while (!done) {
            // ensure client that we are ready for the first part of data
            if (!ServerResponse::TellClientToContinue(socket)) {
                // can't send "HTTP/1.1 100 Continue" to given destination
            }
            try {
                // read the first part
                const std::string &body = request.ExtractBodyByParts(size, socket);
                if (bound_pos != std::string::npos) {
                    // body contains boundary delimiter, adjust start and end position
                    size_t start = body.find(kHTTPEndBlock);
                    start = (start == std::string::npos) ?
                                              0 : start + kHTTPEndBlock.size();
                    std::string delimiter = request.
                                            GetHeaderValue("Content-Type").
                                            substr(bound_pos + kBoundary.size());
                    size_t end = body.rfind(delimiter);
                    // "\r\n" before the delimiter and "--" after == 4
                    if (end == std::string::npos || end <= start) {
                        end = body.size();
                    } else {
                        end -= 4;
                    }
                    const std::string &to_write = body.substr(start, end - start);
                    if (Utils::AppendToFile(to_write,filename)) {
                        size -= body.size();
                        done = end != body.size();
                    }
                } else {
                    // body is simply a file
                    if (Utils::AppendToFile(body, filename)) {
                        // WRONG!
                        done = request.GetDeclaredBodySize() == body.size();
                        return true;
                    }
                }
            } catch (const RequestBodySizeExceedsLimitException &) {
                std::cout << "actual size of body redd is larger that what was expected" << std::endl;
                return true;
            }
        }
    } else {
        const std::string &body = request.GetBody();
        if (bound_pos != std::string::npos) {
            // body contains boundary delimiter
            size_t start = body.find(kHTTPEndBlock) + kHTTPEndBlock.size();
            std::string delimiter = request.GetHeaderValue("Content-Type").
                    substr(bound_pos + kBoundary.size());
            size_t end = body.rfind(delimiter);
            // "\r\n" before the delimiter and "--" after == 4
            end = end > start ? end - 4 : body.size();
            const std::string &to_write = body.substr(start, end - start);
            if (Utils::AppendToFile(to_write,filename)) {
                done = end != body.size();
                return true;
            }
        } else {
            // body is simply a file
            if (Utils::AppendToFile(body, filename)) {
                // WRONG!
                done = request.GetDeclaredBodySize() == body.size();
                return true;
            }
        }
        return false;
    }
    return true;
}