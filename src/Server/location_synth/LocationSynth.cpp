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
    SrchRes res = config_.FindConstLocation(request.getAddress());

    Location synth(*res.location_);
    if (AccessForbidden(res.location_, request.GetMethod())) {
        std::cout << "access forbidden by rule" << std::endl;
        synth.return_code_ = 403;
    } else if (res.location_->return_code_ == 0) {
        if (res.status_ == "found") {
            synth = SynthFoundExact(res.location_, synth);
        } else if (res.status_ == "not found") {
            synth = SynthForNotFound(request, res, synth);
        } else if (res.status_ == "request misconfigured") {
            synth.return_code_ = 400;
        }
    }
    return synth;
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
