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
                                  l_loc_c_it found,
                                  Location &synth,
                                  const std::string &def_res_address) const {
    // literal match between uri and location hierarchy
    if (AccessForbidden(found, request.getMethod())) {
        // limit-except forbids access
        std::cout << "access forbidden by rule" << std::endl;
        synth.return_code_ = 403;
    } else {
        if (found->return_code_ == 0) {
            // no return code is defined
            if (found->return_address_.empty()) {
                // no address-only return directive
                if (CheckFilesystem(found->root_, def_res_address)) {
                    // root directory exists
                    if (found->index_defined_) {
                        // Index is defined explicitly in config
                        l_str_c_it index = FindIndexToSend(found, def_res_address);
                        if (index != found->index_.end()) {
                            // index is found
                            synth.return_code_ = 200;
                            synth.index_.clear();
                            synth.index_.push_back(*index);
                        } else {
                            // index is defined but doesn't exist
                            synth.return_code_ = 403;
                        }
                    } else {
                        // index isn't defined explicitly
                        if (CheckFilesystem(found->root_ + "/index.html",
                                            def_res_address)) {
                            // index.html file present
                            synth.return_code_ = 200;
                        } else {
                            synth.return_code_ = 403;
                        }
                    }
                } else {
                    // root directory doesn't exist
                    std::cout << "open() \"" + def_res_address + found->root_ +
                    "\" failed" << std::endl;
                    synth.return_code_ = 404; // Not Found
                }
            } else {
                // has an address-only "return" directive
                synth.return_code_ = 302;
            }
        }
    }
    return synth;
}

//todo: test me next!!!!
l_str_c_it Server::FindIndexToSend(l_loc_c_it found,
                                   const std::string &def_res_address) const {
    l_str_c_it it = found->index_.begin();
    for (; it != found->index_.end(); ++it) {
        if (it->at(0) != '/' &&
            CheckFilesystem(found->root_ + "/" + *it,def_res_address))
            return it;
        if (CheckFilesystem(found->root_ + *it,def_res_address))
            return it;
    }
    return it;
}

