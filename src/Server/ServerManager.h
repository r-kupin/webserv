/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/03/18 19:57:03 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_LIB_SERVERMANAGER_H
#define WEBSERV_LIB_SERVERMANAGER_H

#include "ServerExceptions.h"

typedef std::list<Server> l_servers;

class ServerManager {
public:
    ServerManager();
    ServerManager(const ServerManager &);
    ServerManager(const Config &config);
    ServerManager &operator=(const ServerManager &);

    ~ServerManager();

    void RunAll();
private:
    l_servers servers_;
};


#endif //WEBSERV_LIB_SERVERMANAGER_H