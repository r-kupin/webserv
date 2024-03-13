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

#include "server/Server.h"
#include "server/MultithreadServer.h"

typedef std::list<Server>               l_servers;
typedef std::list<MultithreadServer>    l_mservers;
typedef std::list<AServer>              l_aservers;

class ServerManager {
public:
    ServerManager();
    ServerManager(const ServerManager &);
    ServerManager(const Config &config);
    ServerManager(const Config &config, int n);
    ServerManager &operator=(const ServerManager &);

    ~ServerManager();

    void RunAllST();
    void RunAllMT();
private:
    ThreadPool  pool_;
    l_servers   servers_;
    l_mservers  mservers_;
};


#endif //WEBSERV_LIB_SERVERMANAGER_H