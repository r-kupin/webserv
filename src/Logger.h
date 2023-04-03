/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Logger.h                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:10:51 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_LOGGER_H
#define WEBSERV_LIB_LOGGER_H

#include <string>

struct Logger {
    Logger();

    enum Where { Out, Err };
    static void log(const std::string& message, Where where);
private:
    std::string logfile;
};


#endif //WEBSERV_LIB_LOGGER_H