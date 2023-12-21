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
    ServerConfiguration::LocConstSearchResult res =
            config_.FindConstLocation(request.getAddress());

    Location synth(*res.location_);
    if (res.status_ == "found") {
        synth = SynthFoundExact(request, *res.location_, synth);
    } else if (res.status_ == "not found") {
        synth = SynthForNotFound(request, *res.location_, synth);
    } else if (res.status_ == "request misconfigured") {
        synth.return_code_ = 400;
    }
    return synth;
}


bool Server::CheckFilesystem(const std::string &address,
                             const std::string &def_res_address) const {
    std::ifstream file((def_res_address + address).c_str());
    if (file.good()) {
        file.close();
        return true;
    }
    file.close();
    return false;
}

//todo check allow and deny of the requester address
bool Server::CheckLimitedAccess(const Location &found, Methods method) const {
    if (found.limit_except_.except_.empty() ||
        found.limit_except_.except_.find(method) !=
        found.limit_except_.except_.end()) {
        if (found.address_ == "/")
            return true;
        else return CheckLimitedAccess(*found.parent_, method);
    }
    return false;
}
