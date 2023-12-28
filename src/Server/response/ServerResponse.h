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

typedef std::list<std::pair<std::string, std::string>> l_str_str;

class ServerResponse {
public:
    class ResponseException : public std::exception {};

    ServerResponse();
    ServerResponse(const ServerResponse &);
    ServerResponse(const ClientRequest &request, const Location &synth);

    ~ServerResponse();

	ServerResponse	&operator=(const ServerResponse &);

//-------------------satic utils------------------------------------------------
    static bool     IsErrorCode(int code);
    static bool     IsRedirectCode(int code);
    static bool     IsOKCode(int code);
    static bool     CheckFilesystem(const std::string &address);
//-------------------init-------------------------------------------------------
    std::string     ComposeTop(const Location &location);
    std::string     NiceTimestamp();
//-------------------functional stuff-------------------------------------------
    void			SendResponse(int dest);



    std::string		GetHeader();

    std::string				            top_;
    std::string                         body_buffer_;
    l_str_str                           headers_;

    static ServerResponse
    CreateResponse(const ClientRequest &request, const Location &root);

protected:
    std::string		FindResponseFileAddr(const Location &where,
									 const std::string &filename);

    std::streampos	GetFileSize();
	std::string		ExtractFilename(std::string &uri);
	void			ResourceNotFound();
	std::ifstream   TryOpenFile(const std::string &filename);

    bool StrEndsWithSlash(std::string uri);


};


#endif //WEBSERV_LIB_SEVRERRESPONSE_H