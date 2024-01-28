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
#include <csignal>
#include "../ServerExceptions.h"

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
                // only post method is acknowledged to contain upload payload
                if (UploadFile(request, found, socket)) {
                    // upload saved successfully
                    synth.SetReturnCode(200);
                    synth.return_custom_message_ = "Upload successfull";
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
