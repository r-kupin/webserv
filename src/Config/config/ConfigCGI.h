/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigCGI.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 16:16:22 by mede-mas          #+#    #+#             */
/*   Updated: 2024/04/30 16:51:40 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGCGI_H
# define CONFIGCGI_H

#include <string>
#include <unordered_map>
#include <vector>

class CGIExecutor {
public:
    static std::string ExecuteCGI(const std::string& scriptPath, const std::unordered_map<std::string, std::string>& envVars);

private:
    static std::vector<std::string> BuildEnvString(const std::unordered_map<std::string, std::string>& envVars);
};



#endif
