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

#ifndef WEBSERV_SERVERMANAGER_H
#define WEBSERV_SERVERMANAGER_H

#include "server/Server.h"

typedef std::vector<Server>       v_servers;
typedef std::vector<pthread_t>    v_threads;

static volatile bool        is_running_ = true;

class ServerManager {
public:
    ServerManager();

    ~ServerManager();

    void            Init(const Config &config);
    void            Start();
    static void     Stop(int signal);
    static void     *StartServer(void *srv);
private:
    v_servers   servers_;
    v_threads   threads_;
};


#endif //WEBSERV_SERVERMANAGER_H