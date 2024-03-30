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

#ifndef WEBSERV_SEVRERRESPONSE_H
#define WEBSERV_SEVRERRESPONSE_H

#include <ostream>
#include "../../connection/request/ClientRequest.h"
#include "../../../Config/location/Location.h"

class ServerResponse {
public:
    class ResponseException : public std::exception {};

    ServerResponse();
    ServerResponse(const std::string &addr, std::ofstream *log_file);

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
    static std::string          ComposeTop(int return_code);
    void                        HandleError(const Location &synth);
    void                        HandleRedirect(const Location &synth);
    void                        GetDefinedErrorPage(const Location &synth);
//-------------------functional stuff-------------------------------------------
    void                        ThrowResponseException(const std::string &msg);
    void                        AddHeader(const std::string &key,
                                          const std::string &value);
private:
    std::string     addr_;
    std::ofstream   *log_file_;

    std::string     top_header_;
    std::string     body_str_;
    m_str_str       headers_;
};

std::ostream &operator<<(std::ostream &os, const ServerResponse &response);

#endif //WEBSERV_SEVRERRESPONSE_H