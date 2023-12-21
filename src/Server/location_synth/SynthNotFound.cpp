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
#include "../ServerExceptions.h"


Location &Server::SynthForNotFound(const ClientRequest &request,
                                   const Location &found,
                                   Location &synth,
                                   const std::string &def_res_address) const {
    // No literal match. Found location will be the closest one.
    // Maybe request asks for a file?
    if (CheckFilesystem(found.root_, def_res_address) &&
        CheckLimitedAccess(found, request.getMethod())) {
        // closest location exists and allows access
        if (found.full_address_ + request.getLastStepUri() ==
            request.getAddress()) { // request asks for a file or subdirectory
            if (CheckFilesystem(found.root_ + request.getLastStepUri(),
                                def_res_address)) {
                synth.return_code_ = 200;
                synth.root_ += request.getLastStepUri();
            } else {
                synth.return_code_ = 404;
            }
        }
    }
    return synth;
}