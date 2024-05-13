/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerCGIHandler.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/05/12 18:29:33 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <cstring>
#include "Server.h"

void Server::ForkCGI(Connection &connection, const std::string &address, const std::string &path_info) const {
    int pipe_out[2];
    if (pipe(pipe_out) == -1) {
        ThrowException("Failed to create pipe for CGI execution");
    }
    connection.cgi_fd_ = pipe_out[0];
    if (!sm_.AddCgiToEpoll(connection.cgi_fd_, connection)) {
        ThrowException("Can't add cgi fd to epoll instance");
    }
    connection.active_cgis_++;
    connection.waiting_for_cgi_ = true;
    pid_t pid = fork();
    // Child process
    if (pid == 0) {
        ChildCGI(connection, address, pipe_out, path_info);
    } else if (pid > 0) {
        close(pipe_out[1]);
    } else {
        ThrowException("fork failed");
    }
}

void Server::HandleCGIinput(Connection &connection) const {
    char    buffer[FILE_BUFFER_SIZE];

    while (is_running_) {
        int bytes_read = read(connection.cgi_fd_, buffer, FILE_BUFFER_SIZE - 1);

        if (bytes_read < 1) {
            NoDataAvailable(bytes_read);
        } else {
            if (send(connection.connection_socket_, buffer, bytes_read,  0) < 0)
                ThrowException("send() returned negative number!");
        }
    }
}

void Server::ChildCGI(const Connection &connection, const std::string &address, const int *pipe_out, const std::string &path_info) const {
    // Redirect stdout to pipe_out (write end of the pipe)
    close(pipe_out[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);

    // copy lines from request to the argument of cgi script
    const v_str &request = connection.request_.GetRawRequest();
    std::vector<char*> args;
    char *constarg[request.size() + 2];

    args.push_back((char *)address.c_str());
    constarg[0] = args[0];
    for (size_t i = 0; i < request.size(); ++i) {
        args.push_back((char *)request[i].c_str());
        constarg[i + 1] = args[i + 1];
    }
    constarg[request.size() + 1] = NULL;

    // Set environment variables
    v_str env;
    env.push_back("REQUEST_METHOD=" +
            Utils::ExtractMethod(connection.request_.GetMethod()));
    env.push_back("QUERY_STRING=" + connection.request_.GetQueryString());
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("CONTENT_TYPE=" +
                        connection.request_.GetHeaderValue("Content-Type"));
    env.push_back("PATH_INFO=" + path_info);
    std::vector<char*> env_char_arr;
    char *constenv[env.size() + 1];
    for (size_t i = 0; i < env.size(); i++) {
        env_char_arr.push_back((char*)env[i].c_str());
        constenv[i] = env_char_arr[i];
    }
    constenv[env.size()] = NULL;

    execve(address.c_str(), constarg, constenv);

    // Exit on failure
    _exit(1);
}
