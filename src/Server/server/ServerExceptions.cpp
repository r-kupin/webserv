/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerExceptions.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/27 12:53:35 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "ServerExceptions.h"

const char *IOFailedException::what() const throw() {
    return exception::what();
}

const char *EwouldblockEagainUpload::what() const throw() {
    return exception::what();
}

const char *ZeroReadUpload::what() const throw() {
    return exception::what();
}

const char *ZeroReadCGI::what() const throw() {
    return exception::what();
}
