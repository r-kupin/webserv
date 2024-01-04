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

// implement redirect if location URI doesn't end with "/"
//Location Server::SynthFoundExact(l_loc_c_it &found, Location &synth,
//                                 bool requesting_file) const {
//    struct stat fileInfo;
//    if (stat(found->root_.c_str(), &fileInfo) == 0) {
//        // something exists at the address
//        if (S_ISREG(fileInfo.st_mode)) {
//            // it's a file
//            if (requesting_file) {
//                synth.body_file_ = found->root_;
//                synth.return_code_ = 200;
//            } else {
//
//            }
//        } else {
//            // it's a directory of something else
//            synth.return_code_ = 403;
//        }
//    } else {
//        std::cout << "open() \"" + found->root_ + "\" failed" << std::endl;
//        synth.return_code_ = 404;
//    }
//    return synth;
//}

//Location Server::SynthFoundExact(l_loc_c_it &found, Location &synth) const {
//    if (Utils::CheckFilesystem(found->root_)) {
//        if (found->index_defined_) {
//            HandleExplicitIndex(found, synth);
//        } else {
//            HandleImplicitIndex(found, synth);
//        }
//    } else {
//        std::cout << "open() \"" + found->root_ + "\" failed" << std::endl;
//        synth.return_code_ = 404;
//    }
//    return synth;
//}

//void Server::HandleImplicitIndex(const l_loc_c_it &found,
//                                 Location &synth) const {
//    if (Utils::CheckFilesystem(found->root_ + "/index.html")) {
////        synth.index_.push_front("index.html");
//        synth.return_code_ = 200;
//    } else {
//        synth.return_code_ = 403;
//    }
//}
//
//void Server::HandleExplicitIndex(l_loc_c_it &found,
//                                 Location &synth) const {
//    const std::string &index_address = FindIndexToSend(found);
//    if (index_address.empty()) {
//        // index defined but found no file(s)
//        synth.return_code_ = 403;
//    } else {
//        synth.return_code_ = 200;
//        synth.body_file_ = index_address;
//    }
//}

void
Server::SynthIndex(Location &synth, const Srch_c_Res &res, int fs_status) const {
    l_loc_c_it found = res.location_;

    const std::string &index_address =
            FindIndexToSend(found,res.leftower_address_);
    if (Utils::CheckFilesystem(index_address) == NOTHING) {
        if (fs_status == NOTHING) {
            std::cout << "\"" + index_address + "\" is not found" << std::endl;
            synth.return_code_ = 404;
        } else {
            std::cout << "index of " + found->root_ + " is forbidden" << std::endl;
            synth.return_code_ = 403;
        }
    } else {
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