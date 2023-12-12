#include <iostream>
#include <algorithm>
#include "ClientRequest.h"

std::string ClientRequest::ExtractLastAddrStep(const std::string& address) {
    unsigned long separator = address.find_last_of('/');

    if (separator == std::string::npos) {
        ThrowException("address has to contain / character", "BadURL");
    }
    return (address.substr(separator));
}

std::string ClientRequest::ExtractAddr(const std::string& url) {
    unsigned long param_separator = url.find_first_of('?');
    unsigned long fragm_separator = url.find_first_of('#');
    if (param_separator == std::string::npos &&
        fragm_separator == std::string::npos) {
        return url;
    } else {
        if (param_separator == std::string::npos)
            return (url.substr(0, fragm_separator));
        else if (fragm_separator == std::string::npos)
            return (url.substr(0, param_separator));
    }
    return (url.substr(0, param_separator));
}

std::string ClientRequest::ExtractFragment(const std::string &url) {
    if (HasFragment(url))
        return (url.substr(url.find_first_of('#') + 1));
    return "";
}

bool ClientRequest::HasFragment(const std::string &url) {
    unsigned long param_separator = url.find_first_of('?');
    unsigned long fragm_separator = url.find_first_of('#');
    if (param_separator == std::string::npos &&
        fragm_separator == std::string::npos) {
        return false;
    } else {
        if (param_separator == std::string::npos)
            return fragm_separator != url.size() - 1;
        else if (fragm_separator == std::string::npos)
            return false;
        else if (param_separator > fragm_separator)
            ThrowException("fragment before url params", "BadURL");
    }
    return fragm_separator != url.size() - 1;
}
