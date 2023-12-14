#include <iostream>
#include <algorithm>
#include "ServerExceptions.h"

std::string get_next_location_address(const std::string &uri) {
    std::string current = uri;
    if (current[0] == '/')
        current = current.substr(1);
    std::string::size_type current_end = current.find_first_of('/');
    return "/" + current.substr(0, current_end);
}

// todo Handle requests fot files like /loc_defined_index_not_exist/pic.jpeg !!!
const Location &recursive_search(const std::string &uri, const Location &start,
                                 std::string &status) {
    if (uri.empty() || uri[0] != '/') {
        status = "uri misconfigured";
        return start;
    } else if (uri != start.address_) {
        std::string first = get_next_location_address(uri);
        std::string remainder = "/";
        if (first != uri)
            remainder = uri.substr(first.size());
        if (first != "/") {
            try {
                const Location &found = *start.FindSublocationByAddress(first);
                return recursive_search(remainder, found, status);
            } catch (const NotFoundException &) {
                status = "not found";
                return start;
            }
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
    const Location &res = recursive_search(uri, conf.GetConstRoot(), status);

    if (status == "found") {
        leftower = "";
    } else {
        leftower = uri.substr(res.full_address_.size());
    }
    return LocSearchResult(res, status, uri, leftower);
}

Server::LocSearchResult::LocSearchResult(const Location &location,
                                         const std::string &status,
                                         const std::string &initialUri,
                                         const std::string &leftowerUri)
        : location_(location),
        status_(status),
        initial_uri_(initialUri),
        leftower_uri_(leftowerUri) {}
