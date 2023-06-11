/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SevrerResponse.h                                   :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/10 13:06:09 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_SEVRERRESPONSE_H
#define WEBSERV_LIB_SEVRERRESPONSE_H


#include <fstream>
#include "ClientRequest.h"
#include "../Config/Location.h"

const static std::string&  kHttpVersion = "HTTP/1.1";
const static std::string&  kHttpPostfix = "\r\n\r\n";
const static size_t        kBufferSize = 1024;

struct ServerResponse {
    ServerResponse(const ClientRequest &request, const Location &main,
                   int http_code);

    ServerResponse &operator=(const ServerResponse &);

    ~ServerResponse();

    std::string GetHeader();
    void SendResponse(int dest);

    const ClientRequest & request_;
    const Location & main_;
    int http_code_;
    bool http_is_error_;
    std::string http_code_deskription_;
    std::string page_addr_;
    std::ifstream response_page_;
protected:
    void FindResponsePageAddr();

    std::streampos GetFileSize();
};


#endif //WEBSERV_LIB_SEVRERRESPONSE_H