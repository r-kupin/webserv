/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ClientMessage.h                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/05/14 13:45:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_CLIENTREQUEST_H
#define WEBSERV_LIB_CLIENTREQUEST_H

/**
 *  Question Mark "?" in a URL:The question mark is used to indicate the
 *  beginning of a query string in a URL. The query string contains
 *  parameters that are sent to the web server, providing additional
 *  information about the request. Parameters are typically in the form of
 *  key-value pairs and are separated by "&" (ampersand) symbols.
 *  For example: https://example.com/page?name=John&age=25
 *  In this URL, the question mark indicates the start of the query string,
 *  and the parameters include "name=John" and "age=25."
 *
 *  Hash/Pound Sign "#" in a URL:The hash or pound sign is used to denote a
 *  fragment identifier in a URL. The fragment identifier refers to a
 *  specific section within a webpage. When a URL contains a "#" symbol
 *  followed by a fragment identifier, the web browser will navigate to and
 *  display the specified section of the webpage.
 *  For example: https://example.com/page#section1 In this URL, the fragment
 *  identifier is "section1," and the browser will scroll to or highlight the
 *  corresponding section on the webpage.
 */

#include <string>
#include <netinet/in.h>
#include <vector>
#include <map>
#include "../../Config/location/LimitExcept.h"

class ClientRequest {
public:
    class RequestException : public std::exception {};

    explicit    ClientRequest(int client_sock);

    Methods getMethod() const;
    const std::string &getAddress() const;
    const std::string &getLastStepUri() const;
    const std::string &getBody() const;
    const std::map<std::string, std::string> &getParams() const;
    const std::map<std::string, std::string> &getHeaders() const;
protected:
    ClientRequest();

    void        Init(int client_sock);
    v_str       ReadFromSocket(int socket);
    Methods     ExtractMethod(const std::string &request);
    std::string ExtractUri(const std::string& request);
    std::string ExtractAddr(const std::string& uri);
    std::string ExtractLastAddrStep(const std::string& uri);
    void        FillHeaders(const v_str &request);
    void        FillUriParams(const std::string &uri);
    bool        HasQuery(const std::string& uri);
    bool        HasFragment(const std::string& uri);
    bool        HasBody(const v_str &request);

    Methods                             method_;
    std::string                         addr_;
    std::string                         last_step_uri_;
    std::string                         body_; //todo
    std::string                         fragment_;
    std::map<std::string, std::string>  params_;
    std::map<std::string, std::string>  headers_;
};


#endif //WEBSERV_LIB_CLIENTREQUEST_H