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

class MultithreadServer : public AServer {
public:
    MultithreadServer(const AServer &server, ThreadPool &pool);
    MultithreadServer(const ServerConfiguration &config, ThreadPool &pool);
    MultithreadServer(const MultithreadServer &server);
protected:
    struct ThreadArgs {
        ThreadArgs(int fd, MultithreadServer *obj);

        int                 fd_;
        MultithreadServer   *obj_;
        pthread_mutex_t     *mutex_; // Mutex to protect std::cout
    };

    bool            AddClientToEpoll(int client_sock, int epoll_fd);// override
    void            AddEpollInstance();// override
    void            HandleRequest(int client_sock);// override
    std::string     HandleRequestInThread(int client_sock);// override
    static void     *ThreadSetup(void *arg);
private:
    ThreadPool                  &pool_;
};


#endif //WEBSERV_LIB_MULTITHREADSERVER_H
