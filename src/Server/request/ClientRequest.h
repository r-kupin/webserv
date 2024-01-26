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
 *  For example: https://example.com/page?name=Johny&age=33
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

#define REQUEST_BUFFER_SIZE 64
#define REQUEST_BODY_METADATA_BUFFER_SIZE 16
#define REQUEST_BODY_FILE_BUFFER_SIZE 1024

#include <string>
#include <netinet/in.h>
#include <vector>
#include <map>
#include "../../Config/location/LimitExcept.h"

class ClientRequest {
public:
    class RequestException : public std::exception {};

    ClientRequest();
    explicit ClientRequest(int client_sock);

    void                Init(int client_sock);
    void                ExtractBody(size_t max_size);

    Methods             GetMethod() const;
    const std::string   &GetAddress() const;
    const std::string   &GetLastStepUri() const;
    const std::string   &GetBody() const;
    const m_str_str     &GetParams() const;
    const m_str_str     &GetHeaders() const;
    bool                HasHeader(const std::string &key) const;
    std::string         GetHeaderValue(const std::string &key) const;
    size_t              GetDeclaredBodySize() const;
    bool                IsCurlRequest() const;

    bool                IsIndexRequest() const;
    const std::string   &GetFragment() const;
    void                SetMethod(Methods method);

    std::string         ExtractBody(size_t size, int socket, std::string &body,
                                    int buffer_size = REQUEST_BUFFER_SIZE) const;
    std::string         ReadBodyPart(size_t size, int socket,
                                     int buffer_size = REQUEST_BUFFER_SIZE) const;
protected:
//-------------------socket-level-----------------------------------------------
    v_str       ReadFromSocket(int socket, std::string &body,
                               int buffer_size = REQUEST_BUFFER_SIZE);
//-------------------vector-of-strings parsed input level----------------------
    void        CheckRequest(const v_str &request);
    bool        HasHeaders(const v_str &request);
    void        FillHeaders(const v_str &request);
//-------------------request main line level------------------------------------
    std::string ExtractUrl(const std::string& request);
    Methods     ExtractMethod(const std::string &request);
//-------------------URL level--------------------------------------------------
    void        CheckURL(const std::string &url);
    bool        HasQuery(const std::string& url);
    bool        HasFragment(const std::string& url);
    std::string ExtractAddr(const std::string& url);
    std::string ExtractQuery(const std::string &url);
    void        FillUrlParams(const std::string &url);
    std::string ExtractFragment(const std::string& url);
    std::string ExtractLastAddrStep(const std::string& address);

    void        ThrowException(const std::string& msg,
                               const std::string &e) const;
private:
    Methods                             method_;
    std::string                         addr_;
    std::string                         addr_last_step_;
    bool                                index_request_;
    std::string                         body_;
    std::string                         fragment_;
    m_str_str                           params_;
    m_str_str                           headers_;
    int                                 socket_;


};

std::ostream &operator<<(std::ostream &os, const ClientRequest &request);

#endif //WEBSERV_LIB_CLIENTREQUEST_H