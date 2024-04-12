/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ResponseStaticUtils.cpp                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/29 11:39:33 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <dirent.h>
#include "ServerResponse.h"

//-------------------code related-----------------------------------------------
std::string ServerResponse::GenerateErrorPage(int code) {
    std::ostringstream page;

    page << "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "    <meta charset=\"UTF-8\">\r\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
    page << "    <title> " << code << " " << Utils::Get().GetCodeDescription(code)
    << " </title>\r\n"
            "    <style>\r\n"
            "        body {\r\n"
            "            background-color: #2e3440;\r\n"
            "            color: #eceff4;\r\n"
            "            font-family: 'Ubuntu Mono', system-ui;\r\n"
            "            text-align: center;\r\n"
            "            margin: 150px;\r\n"
            "        }\r\n"
            "\r\n"
            "        .block {\r\n"
            "            background-color: #3b4252;\r\n"
            "            padding: 20px;\r\n"
            "            border-radius: 10px;\r\n"
            "            margin-bottom: 20px;\r\n"
            "            text-align: justify-all;\r\n"
            "        }\r\n"
            "\r\n"
            "        h1 {\r\n"
            "            color: #5E81AC;\r\n"
            "            font-size: 10em;\r\n"
            "            margin-bottom: 1;\r\n"
            "        }\r\n"
            "\r\n"
            "        p {\r\n"
            "            color: #ECEFF4;\r\n"
            "        }\r\n"
            "    </style>\r\n"
            "</head>\r\n"
            "<body>\r\n"
            "<div class=\"block\">\r\n";
    page << "<h1>" << code << "</h1>\r\n"
            "<hr style=\"border-top: 1px solid #88C0D0; width: 95%;\">\r\n";
    page << "<p>" << Utils::Get().GetCodeDescription(code) << "</p>\r\n"
            "</body>\r\n"
            "</html>\r\n";
    return page.str();
}

std::string ServerResponse::GenerateAutoIndex(const Location &loc) {
    std::ostringstream  page;
    // http://host:port/{dir_addr}
    std::string         dir_addr = loc.dir_to_list_.substr(
                            Utils::FindFirstDifference(loc.dir_to_list_, loc.root_),
                            loc.dir_to_list_.size());
    dir_addr += "/";
    page << "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "    <meta charset=\"UTF-8\">\r\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
    page << "    <title> " << dir_addr << " </title>\r\n"
            "    <style>\r\n"
            "        body {\r\n"
            "            background-color: #2e3440;\r\n"
            "            color: #eceff4;\r\n"
            "            font-family: 'Ubuntu Mono', system-ui;\r\n"
            "            text-align: center;\r\n"
            "            margin: 150px;\r\n"
            "        }\r\n"
            "\r\n"
            "        .block {\r\n"
            "            background-color: #3b4252;\r\n"
            "            padding: 20px;\r\n"
            "            border-radius: 10px;\r\n"
            "            margin-bottom: 20px;\r\n"
            "            text-align: justify-all;\r\n"
            "        }\r\n"
            "\r\n"
            "        h1 {\r\n"
            "            color: #5E81AC;\r\n"
            "            padding: 1;\r\n"
            "            font-size: 2em;\r\n"
            "            margin-bottom: 0;\r\n"
            "            text-align: left;\r\n"
            "        }\r\n"
            "\r\n"
            "        h2 {\r\n"
            "            color: #a3be8c;\r\n"
            "            padding: 1;\r\n"
            "            font-size: 1.5em;\r\n"
            "            margin-bottom: 0.2em;\r\n"
            "            text-align: left;\r\n"
            "        }\r\n"
            "\r\n"
            "        p {\r\n"
            "            color: #ECEFF4;\r\n"
            "        }\r\n"
            "\r\n"
            "        ul {\r\n"
            "            padding: 1em;\r\n"
            "            margin: 1;\r\n"
            "            text-align: left;\r\n"
            "        }\r\n"
            "\r\n"
            "        li {\r\n"
            "            margin-bottom: 10px;\r\n"
            "        }\r\n"
            "\r\n"
            "        a:link {\r\n"
            "            color: #ebcb8b;\r\n"
            "            text-decoration: none;\r\n"
            "        }\r\n"
            "\r\n"
            "        a:visited {\r\n"
            "            color: #d08770;\r\n"
            "            text-decoration: none;\r\n"
            "        }\r\n"
            "    </style>\r\n"
            "</head>\r\n"
            "<body>\r\n"
            "<div class=\"block\">\r\n";
    page << "<h1>Directory Listing" << "</h1>\r\n";
    page << "<h2>" << dir_addr << "</h2>\r\n";
    page << "<hr style=\"border-top: 1px solid #88C0D0; width: 95%;\">\r\n";
    page << "<ul>\r\n";

    // loc.listing_ = location.root + dir_addr
    DIR* dir = opendir(loc.dir_to_list_.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name != ".") {
                std::string fullPath = "http://" + addr_;
                fullPath += dir_addr + name;
                if (Utils::CheckFilesystem(loc.dir_to_list_ + "/" + name) == DIRECTORY) {
                    fullPath += "/";
                    name += "/";
                }
                page << "<li><a href=\"" << fullPath << "\">" ;
                page << name << "</a></li>\r\n";
            }
        }
        closedir(dir);
    } else {
        page << "<p>Error: Unable to open directory</p>\r\n";
    }
    page << "</ul>\r\n"
            "</body>\r\n"
            "</html>\r\n";
    return page.str();
}

