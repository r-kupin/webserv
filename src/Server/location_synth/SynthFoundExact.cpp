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
#include "../Server.h"

void    Server::SynthIndex(Location &synth, const Srch_c_Res &res,
                           int fs_status) const {
    l_loc_c_it found = res.location_;
    const std::string &index_address = FindIndexToSend(found,
                                                       res.leftower_address_);
    if (Utils::CheckFilesystem(index_address) == NOTHING) {
        // index address not found
        if (fs_status != DIRECTORY) {
            // directory, where this index supposed to be doesn't exist
            std::cout << "\"" + index_address + "\" is not found" << std::endl;
            synth.return_code_ = 404;
        } else {
            // directory exists  but there are no index to return
            std::cout << "directory index of " + found->root_ +
                                                "/ is forbidden" << std::endl;
            synth.return_code_ = 403;
        }
    } else {
        // index file found
        synth.return_code_ = 200;
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
            if (Utils::CheckFilesystem(address) == FILE)
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