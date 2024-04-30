/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 03:25:00 by  rokupin          #+#    #+#             */
/*   Updated: 2024/04/30 17:25:04 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CONFIGPARSER_H
#define WEBSERV_CONFIGPARSER_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <list>
#include <fstream>
#include <map>

#include "../server_configuration/ServerConfiguration.h"

/**
 * @brief struct used only during parsing process - to keep redd
 * leftovers left after block parsing
 */
struct RawNode {
	Node node_;
	std::string leftover_;
};

class Config {
public:
	class ConfigException : public std::exception {};

	Config();
	Config(const Config &);
	Config(const std::string &);
	explicit    Config(const Node &confRoot);

	Config &operator=(const Config &);
	friend std::ostream    &operator<<(std::ostream &os, const Config &config);
	~Config();

	const std::string   &getConfPath() const;
	const l_sc          &getConstServers() const;
	l_sc				&GetServers();

	// Adding map to store CGI extensions and their corresponding handler executables
	std::map<std::string, std::string> cgi_handlers;

	// Additional methods for CGI configuration parsing
	void	ParseCGIConfig(std::ifstream& source);

protected:
//-------------------parsing config filestream to the tree of nodes-------------
	void                ParseConfig(std::ifstream &config);
	RawNode             ParseNode(std::ifstream &config,
								  std::string &line_leftover,
								  const v_str &main_directive) const;
	void                PreprocessLine(std::string &line,
									   const std::string &line_leftover) const;
	void                GetChildNode(RawNode &current, std::ifstream &config,
									 std::string &line) const;
	void                GetDirective(std::string &line, RawNode &current,
									 std::ifstream &config) const;
	void                FinishSubNode(std::string &line, RawNode &current,
									  std::ifstream &config) const;
	v_str               ParseDirective(std::string &line, char c) const;
	std::string         HandleQuotedExpression(const std::string &line,
											   size_t &separator,
											   const std::string &word,
											   size_t first_quote) const;
	void                HandleLineLeftower(std::string &line_leftover,
										   std::string &line) const;
	void                FinishMainNode(RawNode &current,
									   std::ifstream &config) const;
//-------------------creating the list of server configurations-----------------
	void                CreateSrvConfigs(Node& root);
	void                CheckServer(Node &node, ServerConfiguration &current);

	void                CheckServerSubnodes(const v_node &subcontexts,
											ServerConfiguration &current);
	void                CheckLocationContextInServer(ServerConfiguration &current,
													 s_str &address_set,
													 v_node_c_it &it) const;
//-------------------getters, exceptions, etc-----------------------------------
	l_sc                &GetServers();
	void                ThrowSyntaxError(const std::string &msg,
										 std::ifstream &config) const;
	void                ThrowSyntaxError(const std::string &msg) const;

private:
	std::string         conf_path_;
	Node                conf_root_;
	l_sc                servers_;

};

#endif //WEBSERV_CONFIGPARSER_H