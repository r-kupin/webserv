/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SynthFoundExact.cpp                                :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:28:25 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
#include <iostream>
#include <algorithm>
#include "../ServerExceptions.h"

Location &Server::SynthFoundExact(const ClientRequest &request,
                                  const Location &found,
                                  Location &synth,
                                  const std::string &def_res_address) const {
    // literal match between uri and location hierarchy
    if (CheckFilesystem(found.root_, def_res_address) &&
        CheckLimitedAccess(found, request.getMethod())) {
        if (found.index_defined_) {
            // Index is defined explicitly in config
            l_str_c_it index = FindIndexToSend(found, def_res_address);
            if (index != found.index_.end()) { // index is found
                synth.return_code_ = 200;
                synth.index_.clear();
                synth.index_.push_back(*index);
            } else { // index is defined but doesn't exist
                synth.return_code_ = 403;
            }
        } else {
//          // todo find parent's index
            synth.return_code_ = 200;
        }
    } else {
        // todo if return code is overridden - handle here or somewhere else?
        synth.return_code_ = 404; // Not Found
    }
    return synth;
}

l_str_c_it Server::FindIndexToSend(const Location &found,
                                   const std::string &def_res_address) const {
    l_str_c_it it = found.index_.begin();
    for (; it != found.index_.end(); ++it) {
        if (it->at(0) != '/' &&
            CheckFilesystem(found.root_ + "/" + *it,def_res_address))
            return it;
        if (CheckFilesystem(found.root_ + *it,def_res_address))
            return it;
    }
    return it;
}

