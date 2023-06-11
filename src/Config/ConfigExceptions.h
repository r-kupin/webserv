/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigExceptions.h                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/01 15:17:04 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_CONFIGEXCEPTIONS_H
#define WEBSERV_LIB_CONFIGEXCEPTIONS_H

#include "Config.h"

class ConfigFileNotFound : public Config::ConfigException {
public:
    const char *what() const throw();
};

class ConfigFileSyntaxError : public Config::ConfigException {
public:
    const char *what() const throw();
};

#endif //WEBSERV_LIB_CONFIGEXCEPTIONS_H
