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
typedef std::map<std::string, std::string>::const_iterator  m_str_str_c_it;

typedef std::list<std::string>                              l_str;
typedef std::list<std::string>::const_iterator              l_str_c_it;

enum Methods {GET, POST, DELETE, UNSUPPORTED};

class Utils {
public:
    class UtilsException : public std::exception {};
    class StatvfsException : public UtilsException {};
    class ConversionException : public UtilsException {};

    static Utils        &Get();

    int                 GetFilesUploaded() const;
    void                IncrementUploadedFiles();
    long                TimeNow() const;
    long                TimeElapsed() const;
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

    static std::string  ExtractMethod(Methods methods);
    static std::string  NbrToString(size_t n);
    static int          FindFirstDifference(const std::string &s1,
                                            const std::string &s2);
    static size_t       StringToULong(const std::string & str);
    static long         StringToNbr(const std::string & str);
    static v_char       StringToVchar(const std::string & str);
    static size_t       FindInBuffer(const char *buffer, size_t buffer_size,
                                     const std::string &pattern);
    static size_t       FindInCharVect(const std::vector<char> &buffer,
                                       const std::string &pattern);
    static void         OutputMap(const m_str_str &map, std::ostream &os);
    static bool         IsPositiveNumber(const std::string &str);
    static std::string  LookupDNS(const std::string &host);
//-------------------HTTP utils-------------------------------------------------
    bool                IsErrorCode(int code);
    bool                IsOKCode(int code);
    bool                IsRedirectCode(int code);
    bool                IsValidHTTPCode(int code);
    std::string         GetCodeDescription(int code);
    std::string         GetMimeType(const std::string &filename);
private:
    long        started_at_;
    int         files_uploaded_;

    m_int_str   err_codes_;
    m_int_str   ok_codes_;
    m_int_str   redirect_codes_;
    m_str_str   mime_types_;

    m_int_str   initializeHttpErrCodes();
    m_int_str   initializeHttpRedirectCodes();
    m_int_str   initializeHttpOKCodes();
    m_str_str   initializeMimeTypes();

    Utils();
};


#endif //WEBSERV_UTILS_H