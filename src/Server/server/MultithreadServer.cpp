/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    MultithreadServer.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/02/22 11:51:56 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <sstream>
#include <cstring>
#include "MultithreadServer.h"
#include "../request/RequestExceptions.h"
#include "ServerExceptions.h"

MultithreadServer::MultithreadServer(const ServerConfiguration &config,
                                     ThreadPool &pool)
: AServer(config), fd_set_(SOMAXCONN, false), pool_(pool) {}

MultithreadServer::MultithreadServer(const AServer &server, ThreadPool &pool)
: AServer(server), fd_set_(SOMAXCONN, false), pool_(pool) {}

MultithreadServer::MultithreadServer(const MultithreadServer &server)
: AServer(server), fd_set_(SOMAXCONN, false), pool_(server.pool_) {}

void MultithreadServer::HandleRequest(int client_sock) {
    ThreadArgs *args = new ThreadArgs(client_sock, this);
    args->mutex_ = new pthread_mutex_t;
    pthread_mutex_init(args->mutex_, NULL);

    pthread_t tid;
    Log("creating thread for socket: " + Utils::NbrToString(client_sock));
    pthread_create(&tid, NULL, &MultithreadServer::ThreadSetup, args);
}

void *MultithreadServer::ThreadSetup(void *arg) {
    pthread_detach(pthread_self());

    ThreadArgs          *thread_args = static_cast<ThreadArgs*>(arg);
    MultithreadServer   *serv = thread_args->obj_;
    int                 sock = thread_args->fd_;


    const std::string   &thread_log = serv->HandleRequestInThread(sock);
    pthread_mutex_lock(thread_args->mutex_);
//    serv->RearmFD(sock, serv->GetEpollFd());
    std::cout << thread_log;
    pthread_mutex_unlock(thread_args->mutex_);
    pthread_mutex_destroy(thread_args->mutex_);
    delete thread_args->mutex_;
    delete thread_args;
    return NULL;
}

std::string MultithreadServer::HandleRequestInThread(int client_sock) {
    std::stringstream   os;
    Location            response_location;
    ClientRequest       request;
    ServerResponse      response(GetConfig().GetServerName(),
                                 GetConfig().GetPort());
    try {
        request.Init(client_sock);
        Log("Handling socket: " +  Utils::NbrToString(client_sock), os);
        Log("Got client request:\n", os);
        os << request << std::endl;
        response_location = ProcessRequest(request, os, client_sock);
        Log("Request processed", os);
    } catch (const HTTPVersionNotSupportedException &) {
        response_location.SetReturnCode(BAD_HTTP_VERSION);
    } catch (const ReadFromSocketFailedException &) {
        response_location.SetReturnCode(FAILED_IO);
    } catch (const ClientRequest::RequestException &) {
        response_location.SetReturnCode(BAD_REQUEST);
    }

    response.ComposeResponse(response_location);
    Log("Prepared response:\n", os);
    os << response << std::endl;
    response.SendResponse(client_sock);
    Log("Response sent", os);
    Log("Done with socket: " +  Utils::NbrToString(client_sock) + "\n", os);
    close(client_sock);
    return os.str();
}

bool MultithreadServer::AddClientToEpoll(int client_sock, int epoll_fd) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = client_sock;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
//    event.events = EPOLLIN | EPOLLOUT;
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) != -1;
}

bool MultithreadServer::RearmFD(int client_sock, int epoll_fd) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = client_sock;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
//    event.events = EPOLLIN | EPOLLOUT;
    return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_sock, &event) != -1;
}

void MultithreadServer::AddEpollInstance() {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = GetSocket();
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
//    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(GetEpollFd(), EPOLL_CTL_ADD, GetSocket(), &event) < 0)
        throw EpollAddFailed();
}

MultithreadServer::ThreadArgs::ThreadArgs(int fd, MultithreadServer *obj)
: fd_(fd), obj_(obj) {}
