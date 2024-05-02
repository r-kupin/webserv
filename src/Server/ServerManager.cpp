/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:15:01 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/02 15:50:15 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Include necessary headers for signal handling and the server manager's declaration.
#include <csignal>

#include "ServerManager.h"

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

// Helper function to extract the extension from a URL
std::string ServerManager::GetExtensionFromURL(const std::string &url) {
	size_t dot_pos = url.rfind('.');
	if (dot_pos == std::string::npos)
		return "";		// No extension found
	return url.substr(dot_pos);
}

// Executes the CGI script and processes its output
void ServerManager::ExecuteCGIScript(Connection &connection, const std::string &cgi_path) {
	// Prepare environment variables
	std::vector<std::string> env_strings; // Store strings for environment
	std::vector<char*> env; // Store pointers for execve
	std::string method = "REQUEST_METHOD=" + connection.getMethod();
	std::string query_string = "QUERY_STRING=" + connection.getQueryString();
	std::string content_type = "CONTENT_TYPE=" + connection.getContentType();
	env_strings.push_back(method);
	env_strings.push_back(query_string);
	env_strings.push_back(content_type);

	for (size_t i = 0; i < env_strings.size(); ++i)
		env.push_back(const_cast<char*>(env_strings[i].c_str()));
	env.push_back(NULL);

	// Create the CGI process (fork, execve)
	pid_t pid = fork();
	if (pid == 0) { // Child process
		// Execute the CGI script
		char *args[] = { const_cast<char*>(cgi_path.c_str()), NULL };
		execve(cgi_path.c_str(), args, env.data());
		_exit(1); // Exit on failure
	} else if (pid > 0) { // Parent process
		// Handle reading CGI output and writing response to the client
	} else {
		// Fork failed, log and handle error
		Log("Failed to fork the CGI process");
	}
}