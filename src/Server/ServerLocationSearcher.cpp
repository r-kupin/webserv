#include <iostream>
#include <algorithm>
#include "Server.h"

std::string get_next_location_address(const std::string &uri) {
    std::string current = uri;
    if (current[0] == '/')
        current = current.substr(1);
    std::string::size_type current_end = current.find_first_of('/');
    return "/" + current.substr(0, current_end);
}

// todo Handle requests fot files like /loc_defined_index_not_exist/pic.jpeg !!!
l_loc_c_it recursive_search(const std::string &uri, l_loc_c_it start,
                            std::string &status) {
    if (uri.empty() || uri[0] != '/') {
        status = "uri misconfigured";
        return start;
    } else if (uri != start->address_) {
        std::string first = get_next_location_address(uri);
        std::string remainder = "/";
        if (first != uri)
            remainder = uri.substr(first.size());
        if (first != "/") {
            l_loc_c_it found = start->FindConstSublocationByAddress(first);
            if (found == start->sublocations_.end()) {
                status = "not found";
                return start;
            }
            return recursive_search(remainder, found, status);
        }
    }
    status = "found";
    return start;
}

Server::LocSearchResult Server::FindLocation(const std::string &uri) const {
    return FindLocation(uri, config_);
}

Server::LocSearchResult Server::FindLocation(const std::string &uri,
                                             const ServerConfiguration &conf) const {
    std::string status, leftower;
    l_loc_c_it res = recursive_search(uri, conf.GetConstRootIt(), status);

    if (status == "found") {
        leftower = "";
    } else {
        leftower = uri.substr(res->full_address_.size());
    }
    return LocSearchResult(res, status, uri, leftower);
}

Server::LocSearchResult::LocSearchResult(l_loc_c_it location,
                                         const std::string &status,
                                         const std::string &initialUri,
                                         const std::string &leftowerUri)
        : location_(location),
        status_(status),
        initial_uri_(initialUri),
        leftower_uri_(leftowerUri) {}
