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

#include <ostream>
#include "../../Config/location/Location.h"
#include "../request/ClientRequest.h"

const static std::string&  kHttpVersion = "HTTP/1.1";
const static size_t        kBufferSize = 1024;

class ServerResponse {
public:
    class ResponseException : public std::exception {};

    ServerResponse();
    ServerResponse(const ServerResponse &);
    ServerResponse(const Location &synth,
                   const std::string &server_name, int port);

    ServerResponse(const std::string &serverName, int port);

    ~ServerResponse();
    void                        ComposeResponse(const Location &synth);
    void                        SendResponse(int dest);

    const std::string           &GetTopHeader() const;
    const std::string           &GetBodyStr() const;
    const m_str_str             &GetHeaders() const;

    ServerResponse              &operator=(const ServerResponse &);
protected:
//-------------------satic utils------------------------------------------------
    static std::string          GeneratePage(int code);
//-------------------init-------------------------------------------------------
    std::string                 ComposeTop(const Location &location);
    void                        HandleError(const Location &synth);
    void                        HandleRedirect(const Location &synth);
    void                        GetDefinedErrorPage(const Location &synth);
//-------------------functional stuff-------------------------------------------
    void                        ThrowResponseException(const std::string &msg);
    void                        AddHeader(const std::string &key,
                                          const std::string &value);
private:
    std::string     top_header_;
    std::string     body_str_;
    std::string     server_name_;
    int             port_;
    m_str_str       headers_;
};

std::ostream &operator<<(std::ostream &os, const ServerResponse &response);

#endif //WEBSERV_LIB_SEVRERRESPONSE_H