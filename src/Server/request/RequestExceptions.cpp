#include <iostream>
#include "RequestExceptions.h"

void ClientRequest::ThrowException(const std::string& msg,
                                   const std::string &e) const {
    std::cout << "ClientRequest: " + msg + "\n" << std::endl;
    if (e == "BadURL")
        throw BadURL();
    else if (e == "BadRequestException")
        throw BadRequestException();
    else if (e == "UnsupportedClientMethodException")
        throw UnsupportedClientMethodException();
    else if (e == "HTTPVersionNotSupportedException")
        throw HTTPVersionNotSupportedException();
    else if (e == "BodyIsTooLarge")
        throw RequestBodySizeExceedsLimitException();
    else if (e == "SendContinue")
        throw SendContinueFailedException();
    else if (e == "ReadFailed")
        throw ReadFromSocketFailedException();
    else if (e == "100ZerosInRow")
        throw MultipleZeroReturns();
    else if (e == "EwouldblockEagain")
        throw EwouldblockEagain();
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

const char *RequestBodySizeExceedsLimitException::what() const throw() {
    return exception::what();
}

const char *SendContinueFailedException::what() const throw() {
    return exception::what();
}

const char *MultipleZeroReturns::what() const throw() {
    return exception::what();
}

const char *EwouldblockEagain::what() const throw() {
    return exception::what();
}
