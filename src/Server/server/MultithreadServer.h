/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    MultithreadServer.h                                :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 11:51:56 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
#ifndef WEBSERV_LIB_MULTITHREADSERVER_H
#define WEBSERV_LIB_MULTITHREADSERVER_H

#include "AServer.h"
#include "../thread_pool/ThreadPool.h"

struct HandlerFunctor {
    int fd_;

    explicit HandlerFunctor(int fd);
    std::string operator()();
};

class MultithreadServer : public AServer {
public:
    MultithreadServer(const AServer &server, ThreadPool &pool);
    MultithreadServer(const ServerConfiguration &config, ThreadPool &pool);
    MultithreadServer(const MultithreadServer &server);
protected:
    void HandleEvents();// override
    void HandleRequest(int client_sock, std::ostream &os);// override
private:
    ThreadPool                  &pool_;
    std::vector<std::string>    logs_buff_;
};


#endif //WEBSERV_LIB_MULTITHREADSERVER_H
