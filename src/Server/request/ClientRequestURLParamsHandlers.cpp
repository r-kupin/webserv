#include <iostream>
#include <algorithm>
#include "ClientRequest.h"

bool querry_ok(const std::string &line, const std::string &url) {
    if (line.find_first_of('=') == std::string::npos ||
        line.find_first_of('=') == 0 ||
        line.find_first_of('=') == line.size() - 1) {
        std::cout << "Client Request:\nurl: " + url + "\n" +
                     "query: " + line + " is incomplete" << std::endl;
        return false;
    }
    return true;
}

std::string ClientRequest::ExtractQuerry(const std::string & url) {
    unsigned long param_separator = url.find_first_of('?');
    unsigned long fragm_separator = url.find_first_of('#');

    if (HasQuery(url)) {
        if (HasFragment(url) || fragm_separator != std::string::npos)
            return url.substr(param_separator + 1,
                              fragm_separator - param_separator - 1);
        return url.substr(param_separator + 1);
    }
    return "";
}

void ClientRequest::FillUrlParams(const std::string &url) {
    std::string query = ExtractQuerry(url);
    for (size_t separator = query.find_first_of('&');
         separator != std::string::npos;
         separator = query.find_first_of('&')) {
        std::string pair = query.substr(0, separator);
        if (!querry_ok(pair, url))
            return;
        std::string name = pair.substr(0, query.find_first_of('='));
        std::string value = pair.substr(query.find_first_of('=') + 1);
        params_.insert(std::make_pair(name, value));
        query = query.substr(pair.size() + 1);
    }
    if (!query.empty()) {
        unsigned long separator = query.find_first_of('=');
        if (querry_ok(query, url)) {
            std::string name = query.substr(0, separator);
            std::string value = query.substr(separator + 1);
            params_.insert(std::make_pair(name, value));
        }
    }
}

bool        ClientRequest::HasQuery(const std::string& url) {
    unsigned long param_separator = url.find_first_of('?');
    unsigned long fragm_separator = url.find_first_of('#');

    if (param_separator == std::string::npos &&
        fragm_separator == std::string::npos) {
        return false;
    } else {
        if (param_separator == std::string::npos) {
            return false;
        } else if (fragm_separator == std::string::npos) {
            return  querry_ok(url.substr(param_separator + 1), url);
        } else if (param_separator > fragm_separator) {
            ThrowException("fragment before url params", "BadURL");
        }
    }
    return querry_ok(url.substr(param_separator + 1,
                                fragm_separator - param_separator - 1),
                     url);
}