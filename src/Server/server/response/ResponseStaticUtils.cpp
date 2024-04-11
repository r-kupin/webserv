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
            "            margin-bottom: 0.2em;\r\n"
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
    std::string         dir_addr = loc.listing_.substr(
                            Utils::FindFirstDifference(loc.listing_, loc.root_),
                            loc.listing_.size());
    if (dir_addr.empty())
        dir_addr = "/";

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
            "            font-size: 2em;\r\n"
            "            margin-bottom: 0.2em;\r\n"
            "        }\r\n"
            "\r\n"
            "        p {\r\n"
            "            color: #ECEFF4;\r\n"
            "        }\r\n"
            "    </style>\r\n"
            "</head>\r\n"
            "<body>\r\n"
            "<div class=\"block\">\r\n";
    page << "<h1>Directory Listing: " << dir_addr << "</h1>\r\n";
    page << "<hr style=\"border-top: 1px solid #88C0D0; width: 95%;\">\r\n";
    page << "<ul>\r\n";

    DIR* dir = opendir(loc.listing_.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name != ".") {
                std::string fullPath = "http://" + addr_ + "/" + name;
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

