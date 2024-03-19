#ifndef WEBSERV_LIB_REQUESTEXCEPTIONS_H
#define WEBSERV_LIB_REQUESTEXCEPTIONS_H

#include "ClientRequest.h"


class FalseCall : public ClientRequest::RequestException {
public:
    FalseCall() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class EwouldblockEagain : public ClientRequest::RequestException {
public:
    EwouldblockEagain() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class ZeroRead : public ClientRequest::RequestException {
public:
    ZeroRead() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class ReadFromSocketFailedException : public ClientRequest::RequestException {
public:
    ReadFromSocketFailedException() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class UnsupportedClientMethodException : public
                                         ClientRequest::RequestException {
public:
    UnsupportedClientMethodException() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class HTTPVersionNotSupportedException : public
                                         ClientRequest::RequestException {
public:
    HTTPVersionNotSupportedException() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class BadURLException : public ClientRequest::RequestException {
public:
    BadURLException() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class BadRequestException : public ClientRequest::RequestException {
public:
    BadRequestException() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

class RequestBodySizeExceedsLimitException : public
                    ClientRequest::RequestException {
public:
    RequestBodySizeExceedsLimitException() :
                    ClientRequest::RequestException() {}

    const char *what() const throw();
};

class SendContinueFailedException : public ClientRequest::RequestException {
public:
    SendContinueFailedException() : ClientRequest::RequestException() {}

    const char *what() const throw();
};

#endif //WEBSERV_LIB_REQUESTEXCEPTIONS_H
