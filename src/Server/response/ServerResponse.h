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

class ServerResponse {
public:
    class ResponseException : public std::exception {};

    ServerResponse();
    ServerResponse(const ServerResponse &);
    ServerResponse(const ClientRequest &request, const Location &root);

    ~ServerResponse();

	ServerResponse	&operator=(const ServerResponse &);

    std::string		GetHeader();
    void			SendResponse(int dest);

    bool location_defined_;
    bool location_root_path_exists_;
    bool location_root_has_index_;

//    const ClientRequest &	request_;
//	std::string 			address_;
//	std::string				querry_parameters_;
//    bool					http_is_error_;
//	bool					request_static_;
//
//	int						http_code_;
//	std::string				http_code_description_;
//    std::string				response_filename_;
//    std::ifstream			response_file_stream_;
    std::map<std::string, std::string> headers_;

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