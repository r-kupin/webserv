/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManager.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/11 12:15:17 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ServerManager.h"

std::map<int, std::string> MakeErrCodesMap() {
    std::map<int, std::string> err_codes;
    err_codes.insert(std::make_pair(400, "Bad Request"));
    err_codes.insert(std::make_pair(401, "Unauthorized"));
    err_codes.insert(std::make_pair(402, "Payment Required"));
    err_codes.insert(std::make_pair(403, "Forbidden"));
    err_codes.insert(std::make_pair(404, "Not Found"));
    err_codes.insert(std::make_pair(405, "Method Not Allowed"));
    err_codes.insert(std::make_pair(406, "Not Acceptable"));
    err_codes.insert(std::make_pair(407, "Proxy Authentication Required"));
    err_codes.insert(std::make_pair(408, "Request Timeout"));
    err_codes.insert(std::make_pair(409, "Conflict"));
    err_codes.insert(std::make_pair(410, "Gone"));
    err_codes.insert(std::make_pair(411, "Length Required"));
    err_codes.insert(std::make_pair(412, "Precondition Failed"));
    err_codes.insert(std::make_pair(413, "Payload Too Large"));
    err_codes.insert(std::make_pair(414, "URI Too Long"));
    err_codes.insert(std::make_pair(415, "Unsupported Media Type"));
    err_codes.insert(std::make_pair(416, "Range Not Satisfiable"));
    err_codes.insert(std::make_pair(417, "Expectation Failed"));
    err_codes.insert(std::make_pair(418, "I'm a teapot"));
    err_codes.insert(std::make_pair(421, "Misdirected Request"));
    err_codes.insert(std::make_pair(422, "Unprocessable Entity"));
    err_codes.insert(std::make_pair(423, "Locked"));
    err_codes.insert(std::make_pair(424, "Failed Dependency"));
    err_codes.insert(std::make_pair(425, "Too Early"));
    err_codes.insert(std::make_pair(426, "Upgrade Required"));
    err_codes.insert(std::make_pair(428, "Precondition Required"));
    err_codes.insert(std::make_pair(429, "Too Many Requests"));
    err_codes.insert(std::make_pair(431, "Request Header Fields Too Large"));
    err_codes.insert(std::make_pair(451, "Unavailable For Legal Reasons"));
    err_codes.insert(std::make_pair(500, "Internal Server Error"));
    err_codes.insert(std::make_pair(501, "Not Implemented"));
    err_codes.insert(std::make_pair(502, "Bad Gateway"));
    err_codes.insert(std::make_pair(503, "Service Unavailable"));
    err_codes.insert(std::make_pair(504, "Gateway Timeout"));
    err_codes.insert(std::make_pair(505, "HTTP Version Not Supported"));
    err_codes.insert(std::make_pair(506, "Variant Also Negotiates"));
    err_codes.insert(std::make_pair(507, "Insufficient Storage"));
    err_codes.insert(std::make_pair(508, "Loop Detected"));
    err_codes.insert(std::make_pair(510, "Not Extended"));
    err_codes.insert(std::make_pair(511, "Network Authentication Required"));
    return err_codes;
}


ServerManager::ServerManager() {}

ServerManager::ServerManager(const ServerManager &other) {
    (void)other;
}

ServerManager &ServerManager::operator=(const ServerManager &other) {
    if (this == &other)
        return *this;
    return *this;
}

ServerManager::~ServerManager() {}

ServerManager::ServerManager(const Config &config)
: kHttpCodes(MakeErrCodesMap()) {
    const v_sconfigs &configs = config.getServers();

    for (size_t i = 0; i < configs.size(); ++i) {
        servers_.push_back(Server(configs[i], &kHttpCodes));
    }
}

void ServerManager::RunAll() {
    for (size_t i = 0; i < servers_.size(); ++i) {
        servers_[i].Start();
    }
}
