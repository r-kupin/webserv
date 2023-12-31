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
#include <ostream>
#include "../request/ClientRequest.h"
#include "../../Config/location/Location.h"

const static std::string&  kHttpVersion = "HTTP/1.1";
const static size_t        kBufferSize = 1024;

class ServerResponse {
public:
    class ResponseException : public std::exception {};

    std::string     top_header_;
    std::string     body_str_;
    std::string     server_name_;
    int             port_;
    m_str_str       headers_;

    ServerResponse();
    ServerResponse(const ServerResponse &);
    ServerResponse(const ClientRequest &request,
                   const Location &synth,
                   const std::string &server_name, int port);

    ~ServerResponse();

	ServerResponse	&operator=(const ServerResponse &);

//-------------------satic utils------------------------------------------------
    static bool                 IsErrorCode(int code);
    static bool                 IsOKCode(int code);
    static bool                 IsRedirectCode(int code);
    static const std::string    &GetCodeDescription(int code);
    static std::string          GeneratePage(int code);
//-------------------init-------------------------------------------------------
    void                        ComposeResponse(const Location &synth);
    std::string                 ComposeTop(const Location &location);
    void                        HandleError(const Location &synth);
    void                        HandleRedirect(const Location &synth);
    void                        GetDefinedErrorPage(const Location &synth);
//-------------------functional stuff-------------------------------------------
    void                        SendResponse(int dest);
    void                        ThrowResponseException(const std::string &msg);
    void                        AddHeader(const std::string &key,
                                          const std::string &value);
};

std::ostream &operator<<(std::ostream &os, const ServerResponse &response);

#endif //WEBSERV_LIB_SEVRERRESPONSE_H