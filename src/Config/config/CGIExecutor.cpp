/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecutor.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 16:19:13 by mede-mas          #+#    #+#             */
/*   Updated: 2024/04/30 17:34:25 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigCGI.h"
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <map>

std::string CGIExecutor::ExecuteCGI(const std::string& scriptPath, const std::map<std::string, std::string>& envVars) {
	int fd[2];
	pipe(fd);
	pid_t pid = fork();

	// Child process
	if (pid == 0) {
		// Close read end of the pipe
		close(fd[0]); 
		// Redirect stdout to the write end of the pipe
		dup2(fd[1], STDOUT_FILENO);

		// Convert envVars to char** for execve
		std::vector<std::string> envStrings = BuildEnvString(envVars);
		std::vector<char*> envp;
		for (auto& s : envStrings) envp.push_back(const_cast<char*>(s.c_str()));
		envp.push_back(nullptr);

		// Assuming no arguments to the script
		char* args[] = { nullptr };
		execve(scriptPath.c_str(), args, envp.data());
		// Exit if execve fails
		exit(EXIT_FAILURE);

		// Parent process
	} else if (pid > 0) {
		// Close write end of the pipe
		close(fd[1]);

		// Read output from CGI script
		char buffer[1024];
		std::string output;
		ssize_t count;
		while ((count = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[count] = '\0';
			output += buffer;
		}
		close(fd[0]);
		// Wait for child to finish
		waitpid(pid, NULL, 0);
		return output;
	}
	// Return empty string if fork fails
	return "";
}

std::vector<std::string> CGIExecutor::BuildEnvString(const std::map<std::string, std::string>& envVars) {
	std::vector<std::string> result;
	// Use explicit iterator type instead of 'auto'
	for (std::map<std::string, std::string>::const_iterator iter = envVars.begin(); iter != envVars.end(); ++iter) {
		// Access elements using iterator
		result.push_back(iter->first + "=" + iter->second);
	}
	return result;
}