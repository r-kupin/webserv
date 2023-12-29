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

    page << "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    page << "    <title> " << code << " " << GetCodeDescription(code) << " </title>\n"
            "    <style>\n"
            "        body {\n"
            "            background-color: #2e3440;\n"
            "            color: #eceff4;\n"
            "            font-family: 'Helvetica', sans-serif;\n"
            "            text-align: center;\n"
            "            margin: 100px;\n"
            "        }\n"
            "\n"
            "        h1 {\n"
            "            color: #bf616a;\n"
            "            font-size: 10em;\n"
            "            margin-bottom: 0.2em;\n"
            "        }\n"
            "\n"
            "        p {\n"
            "            color: #81a1c1;\n"
            "        }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n";
    page << "<h1> " << code << " </h1>\n";
    page << GetCodeDescription(code) << "\n"
                                        "</body>\n"
                                        "</html>\n";
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
