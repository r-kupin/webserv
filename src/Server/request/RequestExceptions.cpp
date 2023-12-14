#include <iostream>
#include "RequestExceptions.h"

void ClientRequest::ThrowException(const std::string& msg,
                                   const std::string &e) const {
    std::cout << "ClientRequest: " + msg << std::endl;

    if (e == "BadURL")
        throw BadURL();
    else if (e == "BadRequestException")
        throw BadRequestException();
    else if (e == "UnsupportedClientMethodException")
        throw UnsupportedClientMethodException();
    else if (e == "HTTPVersionNotSupportedException")
        throw HTTPVersionNotSupportedException();
}

const char *ReadFromSocketFailedException::what() const throw() {
    return exception::what();
}

const char *UnsupportedClientMethodException::what() const throw() {
    return exception::what();
}

const char *HTTPVersionNotSupportedException::what() const throw() {
    return exception::what();
}

const char *BadURL::what() const throw() {
    return exception::what();
}

const char *BadRequestException::what() const throw() {
    return exception::what();
}
