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
#include <ctime>
#include <iomanip>
#include "ServerResponse.h"

//-------------------code related-----------------------------------------------
bool ServerResponse::IsErrorCode(int code) {
    return ErrPage::kHttpErrCodes.find(code) != ErrPage::kHttpErrCodes.end();
}

bool ServerResponse::IsOKCode(int code) {
    return Location::kHttpOkCodes.find(code) != Location::kHttpOkCodes.end();
}

bool ServerResponse::IsRedirectCode(int code) {
    return Location::kHttpRedirectCodes.find(code) !=
           Location::kHttpRedirectCodes.end();
}

const std::string    &ServerResponse::GetCodeDescription(int code) {
    if (IsErrorCode(code)) {
        return ErrPage::kHttpErrCodes.find(code)->second;
    } else if (IsOKCode(code)) {
        return Location::kHttpOkCodes.find(code)->second;
    }
    throw ResponseException();
}
//-------------------file related-----------------------------------------------
bool ServerResponse::CheckFilesystem(const std::string &address) {
    std::ifstream file(address.c_str());
    if (file.good()) {
        file.close();
        return true;
    }
    file.close();
    return false;
}

std::string ServerResponse::FileToString(const std::string &address) {
    std::ifstream file(address.c_str());

    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

std::string ServerResponse::GeneratePage(int code) {
    std::ostringstream page;

    page << "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "    <meta charset=\"UTF-8\">\r\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
    page << "    <title> " << code << " " << GetCodeDescription(code) << " </title>\r\n"
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
    page << "<p>" << GetCodeDescription(code) << "</p>\r\n"
            "</body>\r\n"
            "</html>\r\n";
    return page.str();
}

std::string ServerResponse::IntToString(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

//-------------------time related-----------------------------------------------
std::string ServerResponse::NiceTimestamp() {
    time_t timestamp = std::time(NULL);
    struct tm *timeinfo = gmtime(&timestamp);

    if (timeinfo == NULL) {
        return "timestamp conversion failed...";
    }

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return buffer;
}
