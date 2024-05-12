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
#include "Server.h"

void Server::ForkCGI(Connection &connection, const std::string &address) const {
    int pipe_out[2];
    if (pipe(pipe_out) == -1) {
        ThrowException("Failed to create pipe for CGI execution");
    }
    connection.cgi_fd_ = pipe_out[0];
    if (!sm_.AddCgiToEpoll(connection.cgi_fd_, &connection)) {
        ThrowException("Can't add cgi fd to epoll instance");
    }
    connection.active_cgis_++;
    connection.waiting_for_cgi_ = true;
    pid_t pid = fork();
    // Child process
    if (pid == 0) {
        ChildCGI(connection, address, pipe_out);
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

void Server::ChildCGI(const Connection &connection, const std::string &address,
                      const int *pipe_out) const {
    // Redirect stdout to pipe_out (write end of the pipe)
    close(pipe_out[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);

    // Set environment variables
    std::vector<std::string> env_strings;
    std::vector<char*> env;
    std::string method = "REQUEST_METHOD=" + Utils::ExtractMethod(connection.request_.GetMethod());
    std::string query_string = "QUERY_STRING=" + connection.request_.GetQueryString();
    env_strings.push_back(method);
    env_strings.push_back(query_string);
    if (connection.request_.HasHeader("Content-Type")) {
        env_strings.push_back(connection.request_.GetHeaderValue("Content-Type"));
    }

    for (size_t i = 0; i < env_strings.size(); ++i)
        env.push_back(const_cast<char*>(env_strings[i].c_str()));
    env.push_back(NULL);

    // Execute the CGI script
    char *args[] = { const_cast<char*>(address.c_str()), NULL };
    execve(address.c_str(), args, env.data());

    // Exit on failure
    _exit(1);
}
