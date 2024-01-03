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

/**
 * Depending on compliance between what was requested and what is being found
 * creates a synthetic location - a copy of the location that was found, but
 * with altered return code, and ...
 * @param request
 * @return not-exact copy of a location found
 */
Location Server::SynthesizeHandlingLocation(const ClientRequest &request) {
    Srch_c_Res res = config_.FindConstLocation(request.getAddress());
    l_loc_c_it found = res.location_;
    Location synth(*found);

    if (AccessForbidden(found, request.GetMethod())) {
        // limit_access rule prohibits request
        std::cout << "access forbidden by rule" << std::endl;
        synth.return_code_ = 403;
    } else if (found->return_code_ == 0) {
        // return redirection rule isn't set
        if (request.index_request_) {
            SynthIndex(synth, res);
        } else {
            SynthFile(synth, res, request);
        }
    }
    return synth;
}

void Server::SynthFile(Location &synth, const Srch_c_Res &res,
                       const ClientRequest &request) const {
    const l_loc_c_it &found = res.location_;
    // request's address part of URI has an address after last "/" check with
    // leftower-address
    switch (Utils::CheckFilesystem(found->root_ + res.leftower_address_)) {
        case DIRECTORY :
            // redirect to index request
            synth.return_code_ = 301;
            synth.return_internal_address_ = request.addr_ + "/";
            break;
        case FILE :
            // requested file exists
            synth.body_file_ = found->root_;
            synth.return_code_ = 200;
            break;
        case ELSE:
            // requested filesystem entry is neither file nor directory
            std::cout << found->root_ + "is neither a file nor a "
                                        "directory.. I don't know what "
                                        "to do with it.." << std::endl;
            synth.return_code_ = 500;
            break;
        default:
            std::cout << "open() \"" + found->root_ + "\" failed" << std::endl;
            synth.return_code_ = 404;
            return;
    }
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
