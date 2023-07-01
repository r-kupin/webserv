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
    ServerResponse(const ClientRequest &request,
				   const Location &root);

    ~ServerResponse();

	ServerResponse	&operator=(const ServerResponse &);

    std::string		GetHeader();
    void			SendResponse(int dest);

    const ClientRequest &	request_;
	std::string 			uri_;
	std::string				querry_parameters_;
    bool					http_is_error_;
	bool					request_static_;

	int						http_code_;
	std::string				http_code_description_;
    std::string				response_filename_;
    std::ifstream			response_file_stream_;
protected:
    std::string		FindResponseFileAddr(const Location &where,
									 const std::string &filename);
	const Location &FindLocation(const std::string &uri, const Location &start,
								 bool &success);

    std::streampos	GetFileSize();
	std::string		ExtractFilename(std::string &uri);
	void			ResourceNotFound();
	std::string		ExtractParams(std::string given_uri);
	std::ifstream TryOpenFile(const std::string &filename);
};


#endif //WEBSERV_LIB_SEVRERRESPONSE_H