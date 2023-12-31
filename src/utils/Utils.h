/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    Utils.h                                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/31 14:45:45 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_UTILS_H
#define WEBSERV_LIB_UTILS_H

#include <set>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <ostream>

typedef std::set<std::string>::const_iterator               s_str_c_it;

typedef std::vector<std::string>                            v_str;
typedef std::vector<std::string>::const_iterator            v_str_c_it;
typedef std::vector<v_str>::const_iterator                  vstr_vstr_c_it;

typedef std::map<int, std::string>                          m_codes;
typedef std::map<std::string, std::string>                  m_str_str;
typedef std::map<std::string, std::string>::const_iterator  m_str_str_c_it;

typedef std::list<std::string>                              l_str;
typedef std::list<std::string>::const_iterator              l_str_c_it;

class Utils {
public:
    static void         OutputMap(const m_str_str &map, std::ostream &os);
    static bool         CheckFilesystem(const std::string &address);
    static std::string  FileToString(const std::string &address);
    static std::string  NiceTimestamp();
    static std::string  IntToString(size_t n);
private:
    Utils();
};


#endif //WEBSERV_LIB_UTILS_H