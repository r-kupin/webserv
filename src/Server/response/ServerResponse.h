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
#include "../request/ClientRequest.h"
#include "../../Config/location/Location.h"

const static std::string&  kHttpVersion = "HTTP/1.1";
const static std::string&  kHttpPostfix = "\r\n\r\n";
const static size_t        kBufferSize = 1024;

typedef std::list<std::pair<std::string, std::string> > l_str_str;

class ServerResponse {
public:
    class ResponseException : public std::exception {};

    std::string     top_;
    std::string     body_buffer_;
    l_str_str       headers_;

    ServerResponse();
    ServerResponse(const ServerResponse &);
    ServerResponse(const ClientRequest &request, const Location &synth);

    ~ServerResponse();

	ServerResponse	&operator=(const ServerResponse &);

//-------------------satic utils------------------------------------------------
    static bool                 IsErrorCode(int code);
    static bool                 IsOKCode(int code);
    static bool                 IsRedirectCode(int code);
    static const std::string    &GetCodeDescription(int code);
    static bool                 CheckFilesystem(const std::string &address);
//-------------------init-------------------------------------------------------
    std::string                 ComposeTop(const Location &location);
    std::string                 NiceTimestamp();
    void                        GetDefinedErrorPage(const Location &synth);
    static std::string          GeneratePage(int code);
//-------------------functional stuff-------------------------------------------
    void			SendResponse(int dest);

    static std::string FileToString(const std::string &address);
};


#endif //WEBSERV_LIB_SEVRERRESPONSE_H