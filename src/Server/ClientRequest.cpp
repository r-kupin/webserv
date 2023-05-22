/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ClientMessage.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/14 13:45:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include "ClientRequest.h"
#include "ServerExceptions.h"

const int BUFFER_SIZE = 1024;

ClientRequest::ClientRequest(int client_sock) {

    char buffer[BUFFER_SIZE];
    int num_bytes;

    // Read client request from socket
    num_bytes = read(client_sock, buffer, BUFFER_SIZE);
    if (num_bytes < 0)
        throw ReadFromSocketFailedException();
    // Add null terminator to buffer
    buffer[num_bytes] = '\0';
    request_ = std::string(buffer);

    std::string method = request_.substr(0, request_.find(' '));
    if (method == "GET") {
        method_ = GET;
    } else if  (method == "POST") {
        method_ = POST;
    } else if  (method == "DELETE") {
        method_ = DELETE;
    } else
        throw UnsupportedClientMethodException();

    uri_ = request_.substr(
            request_.find(' ') + 1,
            request_.find(' ',
            request_.find(' ') + 1) - request_.find(' ') - 1);

    if (request_.substr(request_.find("HTTP/") + 5,3) != "1.1")
        throw HTTPVersionNotSupportedException();

    host_ = request_.substr(request_.find("Host:") + 5,
                            request_.find('\n'));
}
