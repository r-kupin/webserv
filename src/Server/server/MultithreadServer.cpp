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
: AServer(config), pool_(pool), thread_n_(0), file_("srv_output") {}

MultithreadServer::MultithreadServer(const AServer &server, ThreadPool &pool)
: AServer(server), pool_(pool), thread_n_(0), file_("srv_output") {}

MultithreadServer::MultithreadServer(const MultithreadServer &server)
: AServer(server), pool_(server.pool_), thread_n_(0), file_("srv_output") {}

void MultithreadServer::HandleRequest(int client_sock) {
        ThreadArgs *args = new ThreadArgs(client_sock, ++thread_n_, this);
        args->mutex_ = new pthread_mutex_t;
        pthread_mutex_init(args->mutex_, NULL);

        pthread_t tid;
        Log("creating thread #" + Utils::NbrToString(thread_n_) +
            " for socket: " + Utils::NbrToString(client_sock));
        pthread_create(&tid, NULL, &MultithreadServer::ThreadSetup, args);
}

void *MultithreadServer::ThreadSetup(void *arg) {
    pthread_detach(pthread_self());

    ThreadArgs          *thread_args = static_cast<ThreadArgs*>(arg);
    MultithreadServer   *serv = thread_args->obj_;
    int                 sock = thread_args->fd_;

    const std::string   &thread_log = serv->HandleRequestInThread(sock) +
            "Thread #" + Utils::NbrToString(thread_args->thread_n_) + " done";
    pthread_mutex_lock(thread_args->mutex_);
    serv->file_ << thread_log << std::endl;
    pthread_mutex_unlock(thread_args->mutex_);
    pthread_mutex_destroy(thread_args->mutex_);
    delete thread_args->mutex_;
    delete thread_args;
    return NULL;
}

std::string MultithreadServer::HandleRequestInThread(int client_sock) {
    std::stringstream   os;

    while (true) {
        Location        response_location;
        ClientRequest   request;
        ServerResponse  response(GetConfig().GetServerName(),
                                 GetConfig().GetPort());
        try {
            request.Init(client_sock);
            Log("Handling socket: " + Utils::NbrToString(client_sock));
            Log("Got client request:\n");
            os << request << std::endl;
            response_location = ProcessRequest(request, os, client_sock);
            Log("Request processed");
        } catch (const HTTPVersionNotSupportedException &) {
            response_location.SetReturnCode(BAD_HTTP_VERSION);
        } catch (const MultipleZeroReturns &) {
            Log("Nothing left to read");
            epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
            close(client_sock);
            return os.str();
        } catch (const ReadFromSocketFailedException &) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                Log("errno == EAGAIN || errno == EWOULDBLOCK");
                break;
            } else {
                Log("read operation failed, stopping request processing."
                    "Response will not be sent back.");
                break;
            }
        } catch (const ClientRequest::RequestException &) {
            response_location.SetReturnCode(BAD_REQUEST);
        }

        response.ComposeResponse(response_location);
        Log("Prepared response:\n");
        os << response << std::endl;
        response.SendResponse(client_sock);
        Log("Response sent");
    }
    epoll_ctl(GetEpollFd(), EPOLL_CTL_DEL, client_sock, NULL);
    close(client_sock);
    return os.str();
}

bool MultithreadServer::AddClientToEpoll(int client_sock, int epoll_fd) {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = client_sock;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET /*| EPOLLONESHOT*/;
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) != -1;
}

void MultithreadServer::AddEpollInstance() {
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = GetSocket();
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(GetEpollFd(), EPOLL_CTL_ADD, GetSocket(), &event) < 0)
        throw EpollAddFailed();
}

MultithreadServer::ThreadArgs::ThreadArgs(int fd, int threadN, MultithreadServer *obj)
        : fd_(fd),
          thread_n_(threadN),
          obj_(obj) {}
