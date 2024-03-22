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

#ifndef WEBSERV_SERVERMANAGER_H
#define WEBSERV_SERVERMANAGER_H

#include "server/Server.h"

typedef std::list<Server>               l_servers;

class ServerManager {
public:
    ServerManager();
    ServerManager(const Config &config);

    ~ServerManager();

    void RunAll();
private:
    l_servers   servers_;
};


#endif //WEBSERV_SERVERMANAGER_H