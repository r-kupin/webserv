/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/04 19:41:26 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Include necessary headers for signal handling and the server manager's declaration.
#include "ServerManager.h"
#include "connection/Connection.h"
#include <csignal>
#include <unistd.h>		// for fork and pipe
#include <sys/wait.h>	// for waitpid

// Default constructor for the ServerManager class.
ServerManager::ServerManager() {}

// Destructor for the ServerManager class.
ServerManager::~ServerManager() {}

// Starts to handle incoming connections by monitoring of the epoll_fd with
// epoll_wait() block. Runs indefinitely while it receives interruption signal
void ServerManager::Start() {
	Log("Starting servers... ");
	while (is_running_)
		EventLoop();
	Cleanup();
	Log("Server stopped.");
}

// Signal handler method for stopping all servers.
// It checks if the signal received is SIGINT or SIGSTOP, and if so,
// sets a flag to indicate that the servers should stop running.
void ServerManager::Stop(int signal) {
	if (signal == SIGINT || signal == SIGSTOP) {
		std::cout << "\nStopping servers..." << std::endl;
		is_running_ = false;
	}
}

// Helper function to map URL to CGI script path
std::string ServerManager::GetCGIScriptPath(const std::string &url) {
	std::string base_path = "../cgi-bin";
	if (url.find("/cgi-bin/") == 0) {
		// Appending rest of URL after "/cgi-bin/" (8 char - /)
		return base_path + url.substr(8);
	}
	return "";
}

std::string ServerManager::ExecuteCGIScript(Connection &connection, const std::string &cgi_path) {
	// Checking if "/cgi-bin/" present in URL
	std::string cgi_path = GetCGIScriptPath(connection.getUrl());
	if (cgi_path.empty())
		return "";
	
	// Create pipes for communication
	int pipe_out[2];
	if (pipe(pipe_out) == -1) {
		Log("Failed to create pipe for CGI execution");
		return "";
	}

	// Fork to create a child process
	pid_t pid = fork();
	// Child process
	if (pid == 0) {
		// Set environment variables
		std::vector<std::string> env_strings;
		std::vector<char*> env;
		std::string method = "REQUEST_METHOD=" + connection.getMethod();
		std::string query_string = "QUERY_STRING=" + connection.getQueryString();
		std::string content_type = "CONTENT_TYPE=" + connection.getContentType();
		env_strings.push_back(method);
		env_strings.push_back(query_string);
		env_strings.push_back(content_type);

		for (size_t i = 0; i < env_strings.size(); ++i)
			env.push_back(const_cast<char*>(env_strings[i].c_str()));
		env.push_back(NULL);

		// Redirect stdout to pipe_out (write end of the pipe)
		close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[1]);

		// Execute the CGI script
		char *args[] = { const_cast<char*>(cgi_path.c_str()), NULL };
		execve(cgi_path.c_str(), args, env.data());
		// Exit on failure
		_exit(1);
	// Parent process
	} else if (pid > 0) {
		close(pipe_out[1]);
		char buffer[1024];
		std::string output;

		// Read CGI output from the child process
		ssize_t bytes_read;
		while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes_read] = '\0';
			output += buffer;
		}
		close(pipe_out[0]);

		// Wait for the child process to finish
		waitpid(pid, NULL, 0);

		// Return CGI output
		return output;
	} else {
		// Fork failed, log and handle error
		Log("Failed to fork the CGI process");
		return "";
	}
}
