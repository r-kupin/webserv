/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ThreadPool.h                                       :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 11:13:03 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
#ifndef WEBSERV_LIB_THREADPOOL_H
#define WEBSERV_LIB_THREADPOOL_H

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include "../server/MultithreadServer.h"

#define READY 0
#define BUSY 1
#define DONE 2

typedef void *(MServerMethod)(void *);

class ThreadPool;

struct Task {
    Task();
    Task(MultithreadServer *srv, int connectionFd);

    MultithreadServer   *srv_;
    int                 connection_fd_;
};

struct ThreadContext {
    ThreadContext(pthread_t tid, int status, ThreadPool *pool);
//-------------------Static context
    pthread_t   tid_;
    int         status_;
    ThreadPool  *pool_;
//-------------------Input
    Task        current_task_;
//-------------------Result
    std::string log;
};

class ThreadPool {
public:
    //empty pool for single threaded servers
    ThreadPool() {};
    ThreadPool(size_t numThreads);

    ~ThreadPool();

    void        Launch();
    void        AddTask(MultithreadServer::ThreadArgs *args);
protected:
    static void *WorkerThreadRoutine(void *arg);
private:
    std::vector<ThreadContext>  threads_;
    bool                        stop_;
    bool                        start_;
};



#endif //WEBSERV_LIB_THREADPOOL_H
