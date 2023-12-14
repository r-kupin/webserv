#include "ServerExceptions.h"

const char *SocketOpeningFailureException::what() const throw() {
    return exception::what();
}

const char *SocketBindingFailureException::what() const throw() {
    return exception::what();
}

const char *SocketListeningFailureException::what() const throw() {
    return exception::what();
}

const char *AddrinfoCreationFailed::what() const throw() {
    return exception::what();
}

const char *SocketSetOptionsFailureException::what() const throw() {
    return exception::what();
}

const char *EpollCreationFailed::what() const throw() {
    return exception::what();
}

const char *EpollAddFailed::what() const throw() {
    return exception::what();
}

const char *NotFoundException::what() const throw() {
    return exception::what();
}
