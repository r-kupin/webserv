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

// literal match between uri and location hierarchy
Location &Server::SynthFoundExact(const ClientRequest &request,
                                  l_loc_c_it found,
                                  Location &synth) const {
    if (found->return_code_ == 0) {
        SynthNoRedirectionDefined(found, synth);
    }
    return synth;
}

void    Server::SynthNoRedirectionDefined(l_loc_c_it &found,
                                          Location &synth) const {
    if (ServerResponse::CheckFilesystem(found->root_)) {
        if (found->index_defined_) {
            HandleExplicitIndex(found, synth);
        } else {
            HandleImplicitIndex(found, synth);
        }
    } else {
        std::cout << "open() \"" + found->root_ + "\" failed" << std::endl;
        synth.return_code_ = 404;
    }
}

void Server::HandleImplicitIndex(const l_loc_c_it &found,
                                 Location &synth) const {
    if (ServerResponse::CheckFilesystem(found->root_ + "/index.html")) {
        synth.index_.push_front("index.html");
        synth.return_code_ = 200;
    } else {
        synth.return_code_ = 403;
    }
}

void Server::HandleExplicitIndex(l_loc_c_it &found,
                                 Location &synth) const {
    l_str_c_it index = FindIndexToSend(found);
    synth.index_.clear();
    if (index != found->index_.end()) {
        synth.return_code_ = 200;
        synth.index_.push_front(*index);
    } else {
        synth.return_code_ = 403;
    }
}

l_str_c_it Server::FindIndexToSend(l_loc_c_it found) const {
    l_str_c_it it = found->index_.begin();

    for (; it != found->index_.end(); ++it) {
        if (it->at(0) == '/' && ServerResponse::CheckFilesystem(*it))
            return it;
        if (ServerResponse::CheckFilesystem(found->root_ + "/" + *it))
            return it;
    }
    return it;
}