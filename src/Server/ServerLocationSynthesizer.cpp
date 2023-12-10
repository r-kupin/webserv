#include <iostream>
#include <algorithm>
#include "ServerExceptions.h"

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

/**
 * Depending on compliance between what was requested and what is being found
 * creates a synthetic location - a copy of the location that was found, but
 * with altered return code, and ...
 * @param request
 * @return not-exact copy of a location found
 */
Location Server::SynthesizeHandlingLocation(const ClientRequest& request) {
    std::string status;
    const Location &found = FindSublocation(request.address_,
                                            config_.GetRoot(),
                                            status);
    Location synth(found);
    if (status == "found") {
        synth = SynthFoundExact(request, found, synth);
    } else if (status == "not found") {
        synth = SynthForNotFound(request, found, synth);
    } else if (status == "request misconfigured") {
        synth.return_code_ = 400;
    }

    return synth;
}

Location &Server::SynthFoundExact(const ClientRequest &request,
                                  const Location &found,
                                  Location &synth,
                                  const std::string &def_res_address) const {
    // literal match between uri and location hierarchy
    if (CheckFilesystem(found.root_, def_res_address) &&
        CheckLimitedAccess(found, request.method_)) {
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

Location &Server::SynthForNotFound(const ClientRequest &request,
                                   const Location &found,
                                   Location &synth,
                                   const std::string &def_res_address) const {
    // No literal match. Found location will be the closest one.
    // Maybe request asks for a file?
    if (CheckFilesystem(found.root_, def_res_address) &&
        CheckLimitedAccess(found, request.method_)) {
        // closest location exists and allows access
        if (found.full_address_ + request.last_step_uri_ ==
            request.address_) { // request asks for a file or subdirectory
            if (CheckFilesystem(found.root_ + request.last_step_uri_,
                                def_res_address)) {
                synth.return_code_ = 200;
                synth.root_ += request.last_step_uri_;
            } else {
                synth.return_code_ = 404;
            }
        }
    }
    return synth;
}
