/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SevrerResponse.cpp                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/10 13:06:09 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <sstream>
#include "ServerResponse.h"
#include "ServerExceptions.h"

ServerResponse::ServerResponse(const ClientRequest &request,
                               const Location &main, int http_code)
: request_(request), main_(main), http_code_(http_code) {
    if (Location::kHttpOkCodes.find(http_code_) !=
        Location::kHttpOkCodes.end()) {
        http_code_deskription_ = Location::kHttpOkCodes.find(http_code_)->second;
        http_is_error_ = false;
    } else if (ErrPage::kHttpErrCodes.find(http_code_) !=
               ErrPage::kHttpErrCodes.end()) {
        http_code_deskription_ = ErrPage::kHttpErrCodes.find(http_code_)->second;
        http_is_error_ = true;
    } else {
        throw HTTPCodeError();
    }
    FindResponsePageAddr();
}

void ServerResponse::FindResponsePageAddr() {
    if (http_is_error_) {
        const std::set<ErrPage>::iterator &err_page =
                        main_.error_pages_.find(ErrPage(http_code_));
        if (err_page != main_.error_pages_.end()) {
            page_addr_ = kDefaultResPath + main_.root_ + err_page->address_;
        } else {
//            No page for this error code
        }
    } else {
        if (!main_.index_.empty()) {
            for (std::set<std::string>::iterator indx_it = main_.index_.begin();
                 indx_it != main_.index_.end(); ++indx_it) { // Check file like in the config
                page_addr_ = kDefaultResPath + main_.root_ + *indx_it;
            }
        } else {
//            Location has no index
        }
    }
}

ServerResponse &ServerResponse::operator=(const ServerResponse &other) {
    if (this == &other)
        return *this;
    return *this;
}

ServerResponse::~ServerResponse() {}

std::string ServerResponse::GetHeader() {
    std::stringstream header;

    header << kHttpVersion << " " << http_code_ << " " << http_code_deskription_ <<
           " " << kHttpPostfix;

    return header.str();
}

/**
 *  file.seekg(0, std::ios::end);: This line sets the file stream's get
 * pointer to the end of the file using seekg. The first argument 0 indicates
 * an offset of 0 bytes from the end of the file. The second argument
 * std::ios::end specifies that the offset is relative to the end of the file.
 *  std::streampos fileSize = file.tellg();: After setting the get pointer to
 * the end of the file, file.tellg() is called to retrieve the current
 * position of the get pointer. It returns the file size in terms of the
 * number of characters (or bytes) from the beginning of the file to the
 * current get pointer position. The value is assigned to the fileSize
 * variable of type std::streampos.
 *  file.seekg(0, std::ios::beg);: Finally, this line sets the file stream's
 * get pointer to the beginning of the file. It uses seekg again, but with
 * the first argument 0 indicating an offset of 0 bytes from the beginning of
 * the file, and the second argument std::ios::beg specifying that the offset
 * is relative to the beginning of the file.
 */
std::streampos ServerResponse::GetFileSize() {
    response_page_.seekg(0, std::ios::end);
    std::streampos filesize = response_page_.tellg();
    response_page_.seekg(0, std::ios::beg);
    return filesize;
}

void ServerResponse::SendResponse(int dest) {
    // Check file like in the config
    response_page_.exceptions(std::ifstream::failbit);
    response_page_.open(page_addr_.c_str());
    response_page_.exceptions(std::ifstream::badbit);
    std::streampos filesize = GetFileSize();
    std::string header = GetHeader();
    char buffer[kBufferSize];

    //  Send HTTP header
    if (send(dest, header.c_str(), header.size(), 0) >= 0) {
        // Read and send the file data in chunks
        while (filesize > 0) {
            // Determine the number of bytes to read
            size_t bytes_read = std::min(static_cast<size_t>(kBufferSize),
                                         static_cast<size_t>(filesize));
            // Read data from the file
            response_page_.read(buffer, bytes_read);
            // Send the data over the socket
            ssize_t bytesSent = send(dest, buffer, bytes_read, 0);
            if (bytesSent < 0)
                break;
            // Update the remaining file size
            filesize -= bytesSent;
        }
    }
    //  Send header failed
}



