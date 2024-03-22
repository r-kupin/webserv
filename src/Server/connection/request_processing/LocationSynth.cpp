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
#include "../../server/Server.h"

/**
 * Depending on compliance between what was requested and what is being
 * found, server creates a synthetic location - a copy of the location that
 * was found, but with altered return code, and redirect-related fields, or
 * with a body file set.
 * @param request
 * @return not-exact copy of a location found
 */
Location Server::ProcessRequest(ClientRequest &request, std::ostream &os, int socket) {
    Srch_c_Res res = config_.FindConstLocation(request.GetAddress());
    l_loc_c_it found = res.location_;
    Location synth(*found);

    if (RequestBodyExceedsLimit(found, request)) {
        Log("client intended to send too large body");
        synth.SetReturnCode(BODY_TOO_LARGE);
    } else if (AccessForbidden(found, request.GetMethod())) {
        // limit_access rule prohibits request
        Log("access forbidden by rule");
        synth.SetReturnCode(ACCESS_FORBIDDEN);
    } else if (found->return_code_ == 0) {
        // return redirection rule isn't set
        if (!found->uploads_path_.empty()) {
            HandleUpload(request, socket, found, synth, os);
//      } else if (???) {
//          Handle CGI
        } else {
            HandleStatic(request, res, found, synth, os);
        }
    }
    return synth;
}

void Server::HandleStatic(const ClientRequest &request,
                           const Srch_c_Res &res,
                           const l_loc_c_it &found,
                           Location &synth, std::ostream &os) const {
    // It seems like there is no reason to even read the body because it's
    // not clear how should static file handle it ?
    std::string address = found->root_ + res.leftower_address_;
    int fs_status = Utils::CheckFilesystem(address);
    if (fs_status == ELSE) {
        // something exist on specified address, but it is neither a file nor a directory
        Log(address + " is neither a file nor a directory.. "
                      "I don't know what to do with it..");
        synth.SetReturnCode(REQUESTED_FILE_IS_NOT_A_FILE);
    } else {
        if (request.IsIndexRequest()) {
            // request's address part of URI ends with "/"
            SynthIndex(synth, res, fs_status, os);
        } else {
            // request's address part of URI has a filename after the last "/"
            SynthFile(synth, res, fs_status, request.GetAddress(), os);
        }
    }
}

void Server::HandleUpload(ClientRequest &request, int socket, l_loc_c_it &found,
                           Location &synth, std::ostream &os) {
    if (request.GetMethod() == POST) {
        // Try to perform upload
        int upload_status = UploadFile(request, found, socket, os);
        synth.SetReturnCode(upload_status);
        if (synth.return_code_ == OK)
            synth.return_custom_message_ = "Upload successful";
    } else {
        Log("only POST method should be used for upload locations");
        synth.SetReturnCode(UNAPROPRIATE_METHOD);
    }
}

void Server::SynthFile(Location &synth, const Srch_c_Res &res, int fs_status,
                        const std::string &request_address,
                        std::ostream &os) const {
    (void)os;
    const l_loc_c_it &found = res.location_;
    std::string address = found->root_ + res.leftower_address_;
    // request's address part of URI has an address after last "/" check with
    // leftower-address
    if (fs_status == NOTHING) {
        Log("open() \"" + address + "\" failed");
        synth.SetReturnCode(NOT_FOUND);
    } else if (fs_status == DIRECTORY) {
        // redirect to index request
        synth.SetReturnCode(REDIRECT);
        synth.return_internal_address_ = request_address + "/";
    } else {
        synth.body_file_ = address;
        synth.SetReturnCode(OK);
    }
}

bool Server::RequestBodyExceedsLimit(l_loc_c_it found, ClientRequest &request) {
    if (request.GetMethod() == POST || request.GetMethod() == DELETE) {
        if (found->client_max_body_size_ < request.GetDeclaredBodySize()) {
            return true;
        }
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
