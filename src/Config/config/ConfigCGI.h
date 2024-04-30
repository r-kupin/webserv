/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigCGI.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 16:16:22 by mede-mas          #+#    #+#             */
/*   Updated: 2024/04/30 16:17:49 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>

class CGIExecutor {
public:
    static std::string ExecuteCGI(const std::string& scriptPath, const std::unordered_map<std::string, std::string>& envVars);

private:
    static std::string BuildEnvString(const std::unordered_map<std::string, std::string>& envVars);
};

void	Config::ParseCGIConfig(std::ifstream& source) {
	std::string line;
	while (std::getline(source, line)) {
		std::istringstream iss(line);
		std::string key, value, path;
		if (iss >> key >> value >> path) {
			if (key == "CGIHandler") {
				cgi_handlers[value] = path;		// value = extension, path = handler
			}
		}
	}
}
