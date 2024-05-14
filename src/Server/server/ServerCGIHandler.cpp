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

size_t  find_response_endline(const v_char &buff) {
    size_t line_break = Utils::FindInCharVect(buff, "\n");

    if (line_break != std::string::npos && line_break > 0) {
        if (buff[line_break - 1] == '\r')
            line_break--;
    }
    return line_break;
}

bool Server::HandleCGIinput(Connection &connection) const {
    char    buffer[FILE_BUFFER_SIZE];

    while (is_running_) {
        ssize_t bytes_read = read(connection.cgi_fd_, buffer,FILE_BUFFER_SIZE - 1);
        if (bytes_read < 1) {
            NoDataAvailable(bytes_read);
        } else {
            connection.buffer_.insert(connection.buffer_.end(), buffer,
                                      buffer + bytes_read);
            if (!connection.cgi_response_verified_) {
                return VerifyCGIFirstLine(connection);
            } else {
                ssize_t sent = send(connection.connection_socket_, buffer,
                              bytes_read, 0);
                Log(Utils::NbrToString(sent) + " bytes sent " );
                if (sent == bytes_read) {
                    return true;
                } else {
                    ThrowException("send() returned negative number!");
                }
            }
        }
    }
    return false;
}

bool Server::VerifyCGIFirstLine(Connection &connection) const {
    size_t line_break = find_response_endline(connection.buffer_);
    if (line_break != std::string::npos) {
        std::string first_line(connection.buffer_.begin(),
                               connection.buffer_.begin() + line_break);
        std::istringstream iss(first_line);
        std::string http_version, code, description;
        if (iss >> http_version >> code >> description) {
            return CheckParsedFirstLine(connection, http_version, code, description);
        } else {
            Log("CGI sent bad response");
            return false;
        }
    } else {
        return true;
    }
}

bool Server::CheckParsedFirstLine(Connection &connection, const std::string &http_version, const std::string &code, const std::string &description) const {
    if (http_version == "HTTP/1.1" &&
            Utils::Get().IsValidHTTPCode(Utils::StringToNbr(code)) &&
            !description.empty()) {
        connection.cgi_response_verified_ = true;
        size_t sent = send(connection.connection_socket_,
                           connection.buffer_.data(),
                           connection.buffer_.size(), 0);
        connection.cgi_response_verified_ = true;
        if (sent == connection.buffer_.size()) {
            connection.buffer_.clear();
        } else {
            // todo clear part being sent
            Log("not all data is sent");
        }
        return true;
    } else {
        Log("CGI sent bad response");
        return false;
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
