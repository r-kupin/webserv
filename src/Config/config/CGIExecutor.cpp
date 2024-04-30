/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecutor.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 16:19:13 by mede-mas          #+#    #+#             */
/*   Updated: 2024/04/30 16:21:40 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>

std::string CGIExecutor::ExecuteCGI(const std::string& scriptPath, const std::unordered_map<std::string, std::string>& envVars) {
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
		for (auto& s : envStrings) envp.push_back(&s[0]);
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
	return "";
}

std::string CGIExecutor::BuildEnvString(const std::unordered_map<std::string, std::string>& envVars) {
	std::vector<std::string> result;
	for (const auto& [key, value] : envVars) {
		result.push_back(key + "=" + value);
	}
	return result;
}