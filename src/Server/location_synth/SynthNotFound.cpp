/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SynthNotFound.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:29:13 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <algorithm>
#include "../Server.h"

//Location Server::SynthForNotFound(const Srch_c_Res &found, Location &synth) {
//    std::string address = found.location_->root_ + found.leftower_address_;
//    struct stat fileInfo;
//    if (stat(address.c_str(), &fileInfo) == 0) {
//        // something exists at the address
//        if (S_ISREG(fileInfo.st_mode)) {
//            // it's a file
//            synth.body_file_ = address;
//            synth.return_code_ = 200;
//        } else {
//            // it's a directory of something else
//            synth.return_code_ = 403;
//        }
//    } else {
//        std::cout << "open() \"" + address + "\" failed" << std::endl;
//        synth.return_code_ = 404;
//    }
//    return synth;
//}