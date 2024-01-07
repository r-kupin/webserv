/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigFiles.h                                      :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/04 15:31:50 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_CONFIGFILES_H
#define WEBSERV_LIB_CONFIGFILES_H

#include "../../../src/Config/config/Config.h"

static Config simple_index_check("test_resources/index_order/simple_index_check.conf");
static Config conf_index_root_redefined("test_resources/index_order/nginx_root_redefined.conf");
static Config simple("test_resources/simple/nginx.conf");

#endif //WEBSERV_LIB_CONFIGFILES_H
