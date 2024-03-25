#include <iostream>
#include "RequestExceptions.h"

void ClientRequest::ThrowException(const std::string& msg,
                                   const std::string &e) const {
    *log_file_ << "ClientRequest: " + msg + "\n" << std::endl;
    if (e == "BadURL")
        throw BadURLException();
    else if (e == "BadRequestException")
        throw BadRequestException();
    else if (e == "UnsupportedClientMethodException")
        throw UnsupportedClientMethodException();
    else if (e == "HTTPVersionNotSupportedException")
        throw HTTPVersionNotSupportedException();
    else if (e == "BodyIsTooLarge")
        throw RequestBodySizeExceedsLimitException();
    else if (e == "SendContinueFailed")
        throw SendContinueFailedException();
    else if (e == "ReadFailed")
        throw ReadFromSocketFailedException();
    else if (e == "ZeroRead")
        throw ZeroRead();
    else if (e == "EwouldblockEagain")
        throw EwouldblockEagain();
    else if (e == "FalseCall")
        throw FalseCall();
    else if (e == "Stopped")
        throw Stopped();
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

const char *BadURLException::what() const throw() {
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

const char *ZeroRead::what() const throw() {
    return exception::what();
}

const char *EwouldblockEagain::what() const throw() {
    return exception::what();
}

const char *FalseCall::what() const throw() {
    return exception::what();
}

const char *Stopped::what() const throw() {
    return exception::what();
}
