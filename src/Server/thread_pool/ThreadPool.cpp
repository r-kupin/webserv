/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ThreadPool.cpp                                     :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 11:13:03 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/
#include <csignal>
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads)
: stop_(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        pthread_t tid;
        threads_.push_back(ThreadContext(tid, READY, this));
        ThreadContext *current_context_ptr = &threads_.back();
        pthread_create((&current_context_ptr->tid_) , NULL,
                       &WorkerThreadRoutine,
                       current_context_ptr);
    }
}

ThreadPool::~ThreadPool()  {
    stop_ = true;
    for (size_t i = 0; i < threads_.size(); ++i) {
        pthread_join(threads_[i].tid_, NULL);
    }
}

void ThreadPool::AddTask(MultithreadServer::ThreadArgs *args) {

}

void ThreadPool::Launch() {

}

void *ThreadPool::WorkerThreadRoutine(void *arg) {
    ThreadContext   *context = static_cast<ThreadContext *>(arg);
    const bool      &start = context->pool_->start_;
    const bool      &stop = context->pool_->stop_;
    int             &status = context->status_;

    while (!start && !stop)
        usleep(100);
    while (!stop) {
        MultithreadServer   *srv = context->current_task_.srv_;
        int                 socket_fd = context->current_task_.connection_fd_;
        srv->HandleRequestInThread(socket_fd);
        if ()
    }
    return NULL;
}

ThreadContext::ThreadContext(pthread_t tid, int status, ThreadPool *pool)
: tid_(tid), status_(status), pool_(pool) {}

Task::Task(MultithreadServer *srv, int connectionFd)
: srv_(srv), connection_fd_(connectionFd) {}

Task::Task()
: srv_(NULL), connection_fd_(-1) {}
