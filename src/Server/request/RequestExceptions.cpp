#include "RequestExceptions.h"

const char *ReadFromSocketFailedException::what() const throw() {
    return exception::what();
}

const char *UnsupportedClientMethodException::what() const throw() {
    return exception::what();
}

const char *HTTPVersionNotSupportedException::what() const throw() {
    return exception::what();
}

const char *BadURI::what() const throw() {
    return exception::what();
}

const char *BadRequestException::what() const throw() {
    return exception::what();
}
