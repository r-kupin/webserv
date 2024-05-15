/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCGIHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 18:29:03 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/15 18:57:59 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstring>
#include "Server.h"

void Server::ForkCGI(Connection &connection, const std::string &address, const std::string &path_info) const {
	int pipe_stdin[2];			// Pipe to write in CGI stdin
	int pipe_stdout[2];			// Pipe to read from CGI stdout
	if (pipe(pipe_stdout) == -1 || pipe(pipe_stdin) == -1) {
		ThrowException("Failed to create pipe for CGI execution");
	}

	pid_t pid = fork();

	// Child process
	if (pid == 0) {
		ChildCGI(connection, address, pipe_stdin, pipe_stdout, path_info);
	} else if (pid > 0) {
		close(pipe_stdin[0]);		// Close read-end of the stdin pipe
		close(pipe_stdout[1]);		// Close the write end of the stdout pipe

		connection.cgi_stdin_fd_ = pipe_stdin[1];		// Store write-end to write to CGI
		connection.cgi_stdout_fd_ = pipe_stdout[0];		// Store read-end to read from CGI

		if (!sm_.AddCgiToEpoll(connection.cgi_stdout_fd_, connection)) {
			ThrowException("Can't add cgi_stdout_fd to epoll instance");
		}
		connection.active_cgis_++;
		// connection.waiting_for_cgi_ = true;
	} else {
		ThrowException("fork failed");
	}
}

bool Server::HandleCGIinput(Connection &connection) const {
	char    buffer[FILE_BUFFER_SIZE];

	while (is_running_) {
		ssize_t bytes_read = read(connection.cgi_stdout_fd_, buffer,FILE_BUFFER_SIZE - 1);
		if (bytes_read < 1) {
			NoDataAvailable(bytes_read);
		} else {
			connection.cgi_input_buffer_.insert(connection.cgi_input_buffer_.end(),
                                                buffer, buffer + bytes_read);
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
    return false;
}

void Server::ChildCGI(const Connection &connection, const std::string &address, const int *pipe_stdin, const int *pipe_stdout, const std::string &path_info) const {
	// Redirect stdout to pipe_stdout (write end of the pipe)
	dup2(pipe_stdout[1], STDOUT_FILENO);
	close(pipe_stdout[0]);
	close(pipe_stdout[1]);

	// Redirect stdin to pipe_stdin (read end of the pipe)
	dup2(pipe_stdin[0], STDIN_FILENO);
	close(pipe_stdin[0]);
	close(pipe_stdin[1]);

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

bool	Server::SendDataToCGI(Connection &connection, const std::string &data) const {
	ssize_t	bytes_written = write(connection.cgi_stdin_fd_, data.c_str(), data.size());
	if (bytes_written < static_cast<ssize_t>(data.size())) {
		Log("Failed to send all data to CGI stdin");
		return false;
	}
	return true;
}