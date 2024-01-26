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

#define NOTHING 0
#define FILE 1
#define DIRECTORY 2
#define ELSE 3

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
    class UtilsException : public std::exception {};
    class StatvfsException : public UtilsException {};
    class ConversionException : public UtilsException {};

    static int          CheckFilesystem(const std::string &address);
    static bool         FileExists(const std::string &address);
    static bool         CheckSpace(const std::string &address, size_t size);
    static bool         CheckPermissions(const std::string &address);
    static std::string  FileToString(const std::string &address);
    static bool         AppendToFile(const std::string &data,
                                     const std::string &address);

    static std::string  NbrToString(size_t n);
    static size_t       StringToNbr(const std::string & str);
    static std::string  NiceTimestamp();
    static size_t
    FindInBuffer(const char *buffer, size_t buffer_size,
                 const std::string &pattern);

    static bool         IsErrorCode(int code);
    static bool         IsOKCode(int code);
    static bool         IsRedirectCode(int code);
    static bool         IsValidHTTPCode(int code);
    static std::string  GetCodeDescription(int code);
    static void         OutputMap(const m_str_str &map, std::ostream &os);
private:
    static m_codes err_codes;
    static m_codes ok_codes;
    static m_codes redirect_codes;

    static m_codes initializeHttpErrCodes();
    static m_codes initializeHttpRedirectCodes();
    static m_codes initializeHttpOKCodes();

    Utils();
};


#endif //WEBSERV_LIB_UTILS_H