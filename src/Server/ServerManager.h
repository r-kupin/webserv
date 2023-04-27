/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManager.h                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/11 12:15:17 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_SERVERMANAGER_H
#define WEBSERV_LIB_SERVERMANAGER_H


#include "../Config/Config.h"
#include "Server.h"

typedef std::vector<Server> v_servers;
typedef std::vector<ServerConfiguration> v_sconfigs;

class ServerException;

class ServerManager {
public:
    ServerManager();
    ServerManager(const ServerManager &);
    ServerManager(const Config &config);
    ServerManager &operator=(const ServerManager &);

    ~ServerManager();

    void RunAll();
private:
    v_servers servers_;
};


#endif //WEBSERV_LIB_SERVERMANAGER_H