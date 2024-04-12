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

#include "Server.h"

void Server::SynthIndex(Location &synth, const Srch_c_Res &res,
                        int fs_status) const {
    l_loc_c_it found = res.location_;
    const std::string &index_address = FindIndexToSend(found,
                                                       res.leftower_address_);
    if (Utils::CheckFilesystem(index_address) == NOTHING ||
        res.status_ == "not found") {
        // index address not found
        if (fs_status != DIRECTORY) {
            // directory, where this index supposed to be doesn't exist
            Log("\"" + index_address + "\" is not found");
            synth.SetReturnCode(NOT_FOUND);
        } else if (synth.autoindex_) {
            // directory exists  but there are no index to return - generate listing
            synth.listing_ = found->root_ + res.leftower_address_;
        } else {
            // directory exists  but there are no index to return - 403
            Log("directory index of " + found->root_ + "/ is forbidden");
            synth.SetReturnCode(ACCESS_FORBIDDEN);
        }
    } else {
        // index file found
        synth.SetReturnCode(OK);
        synth.body_file_ = index_address;
    }
}

std::string Server::FindIndexToSend(const l_loc_c_it &found,
                                    const std::string &compliment) const {
    const Location &root = found->GetMyRootRef();
    const l_str &indeces = found->GetIndeces();
    std::string address;

    if (!indeces.empty()) {
        // indexes are defined in current or somewhere up in hierarchy
        for (l_str_c_it it = indeces.begin(); it != indeces.end(); ++it) {
            if (it->at(0) == '/') {
                // index with absolute path
                address = root.root_ + *it;
            } else if (compliment.empty()) {
                // found location is explicitly defined in the config
                address = found->root_ + "/" + *it;
            } else {
                // found location is not a last directory in the request path
                address = found->root_ + compliment + "/" + *it;
            }
            if (Utils::CheckFilesystem(address) == COMM_FILE)
                return address;
        }
    } else {
        // no indexes defined, check for default, which is "index.html"
        address = "index.html";
        if (compliment.empty()) {
            address = found->root_ + "/" + address;
        } else {
            address = found->root_ + compliment + "/" + address;
        }
    }
    return address;
}