/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerConfigurationLocationSearch.cpp              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/17 13:31:04 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <algorithm>
#include <iostream>
#include "ServerConfiguration.h"

std::string sc_get_next_location_address(const std::string &addr) {
    std::string current = addr;
    if (current[0] == '/')
        current = current.substr(1);
    std::string::size_type current_end = current.find_first_of('/');
    return "/" + current.substr(0, current_end);
}

l_loc_it sc_recursive_search(const std::string &addr, l_loc_it start,
                               std::string &status) {
    if (addr.empty() || addr[0] != '/') {
        status = "addr misconfigured";
        return start;
    } else if (addr != start->address_) {
        std::string first = sc_get_next_location_address(addr);
        std::string remainder = "/";
        if (first != addr)
            remainder = addr.substr(first.size());
        if (first != "/") {
            l_loc_it found = start->FindSublocationByAddress(first);
            if (found == start->sublocations_.end()) {
                status = "not found";
                return start;
            }
            return sc_recursive_search(remainder, found, status);
        }
    }
    status = "found";
    return start;
}

l_loc_c_it const_recursive_search(const std::string &addr, l_loc_c_it start,
                               std::string &status) {
    if (addr.empty() || addr[0] != '/') {
        status = "addr misconfigured";
        return start;
    } else if (addr != start->address_) {
        std::string first = sc_get_next_location_address(addr);
        std::string remainder = "/";
        if (first != addr)
            remainder = addr.substr(first.size());
        if (first != "/") {
            l_loc_c_it found = start->FindConstSublocationByAddress(first);
            if (found == start->sublocations_.end()) {
                status = "not found";
                return start;
            }
            return const_recursive_search(remainder, found, status);
        }
    }
    status = "found";
    return start;
}

ServerConfiguration::LocSearchResult
ServerConfiguration::FindLocation(const std::string &address) {
    std::string status, leftower;
    l_loc_it res = sc_recursive_search(address, GetRootIt(), status);

    if (status == "found") {
        leftower = "";
    } else {
        leftower = address.substr(res->full_address_.size());
    }
    return LocSearchResult(res, status, address, leftower);
}

ServerConfiguration::LocConstSearchResult
ServerConfiguration::FindConstLocation(const std::string &address) const {
    std::string status, leftower;
    l_loc_c_it res = const_recursive_search(address, GetConstRootIt(), status);

    if (status == "found") {
        leftower = "";
    } else {
        leftower = address.substr(res->full_address_.size());
    }
    return LocConstSearchResult(res, status, address, leftower);
}
