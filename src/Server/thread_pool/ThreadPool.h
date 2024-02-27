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
#include <semaphore.h>

class ThreadPool {
public:
    //empty pool for single threaded servers
    ThreadPool() {};

    ThreadPool(size_t numThreads) : stop(false) {
        (void)numThreads;
//        for (size_t i = 0; i < numThreads; ++i) {
//            pthread_t tid;
//            pthread_create(&tid, NULL, &ThreadPool::workerThread, this);
//            threads.push_back(tid);
//        }
    }

    ~ThreadPool() {
//        stop = true;
//        for (size_t i = 0; i < threads.size(); ++i) {
//            pthread_join(threads[i], NULL);
//        }
    }

    void addTask(void (*function)(int), int fd) {
        (void)function;
        (void)fd;
        (void)stop;
//        {
//            // Protect the task queue with a lock
//            pthread_mutex_lock(&queueMutex);
//            tasks.push(std::make_pair(function, fd));
//            pthread_mutex_unlock(&queueMutex);
//        }
//        // Signal a worker thread that a task is available
//        sem_post(&taskSemaphore);
    }

private:
    static void* workerThread(void* arg) {
        (void)arg;
//        ThreadPool* pool = static_cast<ThreadPool*>(arg);
//        while (true) {
//            sem_wait(&pool->taskSemaphore); // Wait for a task to be available
//            if (pool->stop) break; // Check if thread pool is stopping
//            pthread_mutex_lock(&pool->queueMutex);
//            if (!pool->tasks.empty()) {
//                std::pair<void (*)(int), int> task = pool->tasks.front();
//                pool->tasks.pop();
//                pthread_mutex_unlock(&pool->queueMutex);
//                task.first(task.second); // Execute the task
//            } else {
//                pthread_mutex_unlock(&pool->queueMutex);
//            }
//        }
        return NULL;
    }

    std::vector<pthread_t> threads;
    std::queue<std::pair<void (*)(int), int> > tasks; // Use explicit whitespace to avoid >> parsing issue
    bool stop;
//    pthread_mutex_t queueMutex;
//    sem_t taskSemaphore;
};



#endif //WEBSERV_LIB_THREADPOOL_H
