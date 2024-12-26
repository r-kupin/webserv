/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCGIHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 18:29:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/18 10:37:22 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "Server.h"

bool Server::ForkCGI(Connection &connection, const std::string &address, const std::string &path_info) const {
	int pipe_stdin[2];			// Pipe to write in CGI stdin
	int pipe_stdout[2];			// Pipe to read from CGI stdout
	if (pipe(pipe_stdout) == -1 || pipe(pipe_stdin) == -1) {
		Log("Failed to create pipe for CGI execution");
        return false;
	}

	connection.cgi_pid_ = fork();

	// Child process
	if (connection.cgi_pid_ == 0) {
		ChildCGI(connection, address, pipe_stdin, pipe_stdout, path_info);
	} else if (connection.cgi_pid_ > 0) {
		close(pipe_stdin[0]);		// Close read-end of the stdin pipe
		close(pipe_stdout[1]);		// Close the write end of the stdout pipe

		connection.cgi_stdin_fd_ = pipe_stdin[1];		// Store write-end to write to CGI
		connection.cgi_stdout_fd_ = pipe_stdout[0];		// Store read-end to read from CGI
        if (!sm_.AddCgiToEpoll(connection.cgi_stdout_fd_, connection)) {
            Log("Can't add cgi_stdout_fd to epoll instance");
            return false;
        }
        if (!sm_.AddCgiToEpoll(connection.cgi_stdin_fd_, connection)) {
            Log("Can't add cgi_stdin_fd to epoll instance");
            return false;
        }
        Log("New CGI process. Server: " +
            Utils::NbrToString(connection.server_listening_socket_) +
            ", Client: "  + Utils::NbrToString(connection.connection_socket_) +
            ", CGI stdin: "  + Utils::NbrToString(connection.cgi_stdin_fd_) +
            ", CGI stdout: "  + Utils::NbrToString(connection.cgi_stdout_fd_));
        connection.active_cgis_++;
        sm_.opened_cgi_processes_++;
        connection.waiting_for_cgi_ = true;
	} else {
        Log("Failed to fork CGI process");
        return false;
	}
    return true;
}

int Server::HandleCGIinput(Connection &connection) const {
	char    buffer[FILE_BUFFER_SIZE];

	while (is_running_) {
        std::cout << "input size: " << connection.cgi_input_buffer_.size() <<
        std::endl;
        while (!connection.cgi_input_buffer_.empty()) {
            ssize_t sent = send(connection.connection_socket_,
                                connection.cgi_input_buffer_.data(),
                                connection.cgi_input_buffer_.size(), 0);
            std::cout << "sent: " << sent << std::endl;
            if (sent < 1) {
                if (sigpipe_) {
                    std::cout << "sigpipe true" << std::endl;
                    sigpipe_ = false;
                    return CLIENT_CLOSED_CONNECTION_WHILE_CGI_SENDS_DATA;
                } else {
                    std::cout << "sigpipe false" << std::endl;
                    return CLIENT_CLOSED_CONNECTION_WHILE_CGI_SENDS_DATA;
                }
            } else {
                connection.cgi_input_buffer_.erase(
                        connection.cgi_input_buffer_.begin(),
                        connection.cgi_input_buffer_.begin() + sent);
            }
        }
		ssize_t bytes_read = read(connection.cgi_stdout_fd_, buffer,
                                  FILE_BUFFER_SIZE - 1);
        std::cout << "read: " << bytes_read << std::endl;
        if (bytes_read < 0) {
            return NOT_ALL_DATA_READ_FROM_CGI;
        } else if (bytes_read == 0 ) {
            return ALL_READ_ALL_SENT;
        } else {
			connection.cgi_input_buffer_.insert(connection.cgi_input_buffer_.end(),
                                                buffer, buffer + bytes_read);
        }
    }
    // server stopped
    return -1;
}

int Server::HandleCGIoutput(Connection &connection) const {
    v_char          &what = connection.cgi_output_buffer_;
    int             where = connection.cgi_stdin_fd_;
    ClientRequest   &request = connection.request_;

    if (what.empty()) {
        for (v_str_c_it it = request.GetRawRequest().begin();
             it != request.GetRawRequest().end(); ++it) {
            v_char tmp(it->begin(), it->end());
            what.insert(what.end(), tmp.begin(), tmp.begin() + tmp.size());
            what.push_back('\n');
        }
        if (request.HasHeader("Content-Length") &&
            Utils::StringToULong(request.GetHeaderValue("Content-Length")) > 0L) {
            what.push_back('\n');
            const v_char &body = request.GetBody();
            what.insert(what.end(),
                        body.begin() + 2,
                        body.begin() + body.size());
        }
    }
    while (is_running_ && !what.empty() && ProbeWriteToCGI(what, where)) {
        ssize_t bytes_written = write(where, what.data() + 1, what.size() - 1);
        std::cout << "written to CGI: " << bytes_written << std::endl;
        if (bytes_written < 0) {
            return NOT_ALL_DATA_WRITTEN_TO_CGI;
        } else if (bytes_written == 0) {
            return CGI_CLOSED_INPUT_FD;
        } else {
            what.erase(what.begin(), what.begin() + bytes_written + 1);
        }
    }
    char    buffer[FILE_BUFFER_SIZE];
    ssize_t bytes_read, bytes_written = 0;
    do {
        bytes_read = recv(connection.connection_socket_, buffer,
                                  static_cast<size_t>(FILE_BUFFER_SIZE), 0);
        if (bytes_read > 0) {
            bytes_written = write(where, buffer, bytes_read);
            if (bytes_written == -1) {
                Log("Write to file failed");
                return NOT_ALL_DATA_WRITTEN_TO_CGI;
            }
        }
    }  while (is_running_ && bytes_read > 0 && bytes_written > 0);
    return ALL_DATA_SENT_TO_CGI;
}

void Server::ChildCGI(const Connection &connection, const std::string &address,
                      const int *pipe_stdin, const int *pipe_stdout,
                      const std::string &path_info) const {
	// Redirect stdout to pipe_stdout ('write end' of the pipe)
	if (dup2(pipe_stdout[1], STDOUT_FILENO) == -1)
        ThrowException("Dup failed!");
	close(pipe_stdout[0]);
	close(pipe_stdout[1]);

	// Redirect stdin to pipe_stdin (read end of the pipe)
    if (dup2(pipe_stdin[0], STDIN_FILENO) == -1) {
        ThrowException("Dup failed!");
    }
	close(pipe_stdin[0]);
	close(pipe_stdin[1]);

	// copy lines from request to the argument of cgi script
	std::vector<char*> args;
	char *constarg[2];

	args.push_back((char *)address.c_str());
	constarg[0] = args[0];
	constarg[1] = NULL;

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

    ThrowException("EXECVE failed");
	// Exit on failure
	_exit(1);
}