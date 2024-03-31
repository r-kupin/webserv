/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/31 14:45:04 by  rokupin          #+#    #+#             */
/*   Updated: 2024/03/26 23:03:36 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_UTILS_H
#define WEBSERV_UTILS_H

#include <set>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <ostream>

#define NOTHING 0
#define COMM_FILE 1
#define DIRECTORY 2
#define ELSE 3

typedef volatile const bool                                 v_c_b;

typedef std::set<int>::const_iterator                       s_int_c_it;
typedef std::set<std::string>                               s_str;
typedef std::set<std::string>::const_iterator               s_str_c_it;

typedef std::vector<char>                                   v_char;

typedef std::vector<std::string>                            v_str;
typedef std::vector<std::string>::const_iterator            v_str_c_it;
typedef std::vector<v_str>::const_iterator                  vstr_vstr_c_it;

typedef std::map<int, std::string>                          m_int_str;
typedef std::map<std::string, std::string>                  m_str_str;
typedef std::map<int, std::set<int> >                       m_int_ints;
typedef std::map<std::string, std::string>::const_iterator  m_str_str_c_it;

typedef std::list<std::string>                              l_str;
typedef std::list<std::string>::const_iterator              l_str_c_it;

class Utils {
public:
    class UtilsException : public std::exception {};
    class StatvfsException : public UtilsException {};
    class ConversionException : public UtilsException {};
//-------------------filesystem utils-------------------------------------------
    static int          CheckFilesystem(const std::string &address);
    static bool         FileExists(const std::string &address);
    static bool         CheckSpace(const std::string &address, size_t size);
    static bool         CheckPermissions(const std::string &address);
    static std::string  FileToString(const std::string &address);
    static std::string  DirName(const std::string &address,
                                const std::string &root);
//-------------------string/container utils-------------------------------------
    static std::string  NiceTimestamp();

    static std::string  NbrToString(size_t n);
    static size_t       StringToULong(const std::string & str);
    static long         StringToNbr(const std::string & str);
    static v_char       StringToVchar(const std::string & str);
    static size_t       FindInBuffer(const char *buffer, size_t buffer_size,
                                     const std::string &pattern);
    static size_t       FindInCharVect(const std::vector<char> &buffer,
                                       const std::string &pattern);
    static void         OutputMap(const m_str_str &map, std::ostream &os);
//-------------------HTTP utils-------------------------------------------------
    static bool         IsErrorCode(int code);
    static bool         IsOKCode(int code);
    static bool         IsRedirectCode(int code);
    static bool         IsValidHTTPCode(int code);
    static std::string  GetCodeDescription(int code);
//-------------------Misc-------------------------------------------------------
    static long         TimeNow();
private:
    static m_int_str err_codes;
    static m_int_str ok_codes;
    static m_int_str redirect_codes;

    static m_int_str initializeHttpErrCodes();
    static m_int_str initializeHttpRedirectCodes();
    static m_int_str initializeHttpOKCodes();

    Utils();
};


#endif //WEBSERV_UTILS_H