/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Logger.cpp                                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 04:10:51 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include "Logger.h"

Logger::Logger() {

}

void Logger::log(const std::string &message, Logger::Where where) {
    if (where == Out) {
        std::cout << message << std::endl;
    } else if (where == Err) {
        std::cerr << message << std::endl;
    }
}
