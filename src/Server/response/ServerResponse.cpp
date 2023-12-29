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
#include <iostream>
#include "ServerResponse.h"

ServerResponse::ServerResponse() {}

ServerResponse::ServerResponse(const ClientRequest &request,
                               const Location &synth,
                               const std::string &server_name, int port)
    : server_name_(server_name), port_(port) {
    (void) request;
    ComposeResponse(synth);
}

void ServerResponse::ComposeResponse(const Location &synth) {
    top_header_ = ComposeTop(synth);
    headers_.push_back(std::make_pair("Server", "webserv"));
    headers_.push_back(std::make_pair("Date", NiceTimestamp()));
    if (IsErrorCode(synth.return_code_)) {
        HandleError(synth);
    } else if (IsRedirectCode(synth.return_code_)) {
        HandleRedirect(synth);
    } else {
        body_str_ = FileToString(synth.root_ + "/" + synth.index_.front());
        AddContentRelatedHeaders();
    }
    headers_.push_back(std::make_pair("Connection", "keep-alive"));
}

void ServerResponse::HandleRedirect(const Location &synth) {
    body_str_ = GeneratePage(synth.return_code_);
    AddContentRelatedHeaders();
    if (!synth.return_external_address_.empty()) {
        headers_.push_back(
                std::make_pair("Location", synth.return_external_address_));
    } else if (!synth.return_internal_address_.empty()) {
        headers_.push_back(std::make_pair("Location",
                               "http://" + server_name_ + ":" +
                               IntToString(port_) +
                               synth.return_internal_address_));
    }
}

void ServerResponse::HandleError(const Location &synth) {
    if (synth.HasErrPageForCode(synth.return_code_)) {
        GetDefinedErrorPage(synth);
    } else {
        body_str_ = GeneratePage(synth.return_code_);
    }
    AddContentRelatedHeaders();
}

void ServerResponse::AddContentRelatedHeaders() {
    headers_.push_back(std::make_pair("Content-Type", "text/html"));
    headers_.push_back(std::make_pair("Content-Length",
                                      IntToString(body_str_.size())));
}

void ServerResponse::GetDefinedErrorPage(const Location &synth) {
    std::string address = synth.root_ + "/" +
                        synth.FindErrPageForCode(synth.return_code_)->address_;
    if (CheckFilesystem(address)) {
        body_str_ = FileToString(address);
    } else {
        body_str_ = GeneratePage(synth.return_code_);
    }
}


std::string ServerResponse::ComposeTop(const Location &location) {
    std::ostringstream oss;

    oss << kHttpVersion << " " << location.return_code_ << " ";
    if (!location.return_custom_message_.empty()) {
        oss << location.return_custom_message_;
    } else if (IsErrorCode(location.return_code_)) {
        oss << ErrPage::kHttpErrCodes.find(location.return_code_)->second;
    } else {
        oss << Location::kHttpOkCodes.find(location.return_code_)->second;
    }
    return oss.str();
}

void ServerResponse::SendResponse(int dest) {
    std::stringstream ss;

    ss << top_header_ << "\r\n";
    for (l_str_str_c_it it = headers_.begin();it != headers_.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n" << body_str_;

    std::string response_string = ss.str();
    const char *response_buffer = response_string.c_str();
    size_t response_size = response_string.size();

    if (send(dest, response_buffer, response_size, 0) < 0)
        ThrowResponseException("send() returned negative number!");
}

void    ServerResponse::ThrowResponseException(const std::string& msg) {
    std::cout << "Response creation failed: " << msg << std::endl;
    throw ResponseException();
}

void    output_list(l_str_str list, std::ostream &os) {
    for (l_str_str_c_it it = list.begin(); it != list.end(); ++it) {
        os << "\t[ " << it->first << " ] " << it->second << "\n";
    }
    os << std::endl;
}

std::ostream &operator<<(std::ostream &os, const ServerResponse &response)  {
    os << response.top_header_ << "\n";
    output_list(response.headers_, os);
    os << response.body_str_;
    return os;
}


//bool CanProceedWith(const Location &location, const ClientRequest &request) {
//    if (location.limit_except_.except_.find(request.method_) ==
//        location.limit_except_.except_.end())
//    if (location.return_code_ > 0)
//}
//
//Location &
//SynthesizeHandlingLocation(const std::string &uri, const Location &start, bool &path_exists,
//             bool &loc_defined, bool &loc_defined_index_which_exist,
//             const ClientRequest &request) {
//    (void)request;
//    if (uri != start.address_) {
//        std::string part_uri = uri.substr(1);
//        std::string::size_type end = part_uri.find('/');
//        if (end == std::string::npos)
//            end = uri.size();
//        std::string loc_addr = ("/" + part_uri).substr(0, end + 1);
//
////        std::ifstream directory(start.root_.c_str());
////        if (directory.good()) {
////            path_exists = true;
////            std::ifstream index((start.root_ + "index.html").c_str());
////            if (index.good()) {
////                loc_defined_index_which_exist = true;
////            }
////        }
//        try {
//            const Location &found = start.FindSublocationByAddress(loc_addr);
////            if (CanProceedWith(found, request))
//                return SynthesizeHandlingLocation(uri.substr(end + 1),
//                                    found, path_exists, loc_defined,
//                                    loc_defined_index_which_exist,
//                                    ClientRequest(0));
//        } catch (const NotFoundException &) {
//            return start;
//        }
//    }
//    path_exists = true;
//    loc_defined = true;
//    return start;
//}
//
//ServerResponse  ServerResponse::CreateResponse(const ClientRequest &request,
//                                               const Location &root) {
//
////    bool path_exists = false;
////    bool location_defined = false;
////    bool loc_defined_index_which_exist = false;
////     const Location & main = sc_recursive_search(uri, root, path_exists,
////                                         location_defined, loc_defined_index_which_exist,
////                                         request);
////    (void)main;
////    if (path_exists) {
////
////    }
////    if (location_defined) {
////
////    }
//    return ServerResponse();
//}

//
//ServerResponse::ServerResponse(const ClientRequest &request,
//							   const Location &root)
////: request_(request)
//{
//	address_ = ExtractParams(request.address_);
//	bool path_exists = false;
//	bool location_defined = false;
//	const Location & main = FindSublocation(address_, root, path_exists,
//                                         location_defined);
//	if (path_exists) {
//
//		if (!response_filename_.empty()) {
//			response_filename_ = FindResponseFileAddr(main, response_filename_);
////			response_file_stream_ = TryOpenFile(response_filename_);
//			if (response_file_stream_.is_open()) { // file exists
//				// Ready to send
//			} else { // file not found
//				ResourceNotFound();
//			}
//		} else { // location index
//			std::set<std::string>::iterator it = main.index_.begin();
//			for (;it != main.index_.end(); ++it) {
//				response_filename_ = FindResponseFileAddr(main, *it);
////				response_file_stream_ = TryOpenFile(response_filename_);
//				if (response_file_stream_.is_open()) { // file exists
//					// Ready to send
//				}
//			}
//			if (it == main.index_.end()) { // No Index
//				ResourceNotFound();
//			}
//		}
//	} else {
//		ResourceNotFound();
//	}
//
////    if (Location::kHttpOkCodes.find(http_code_) !=
////												Location::kHttpOkCodes.end()) {
////		http_code_description_ = Location::kHttpOkCodes.find(http_code_)->second;
////        http_is_error_ = false;
////    } else if (ErrPage::kHttpErrCodes.find(http_code_) !=
////												ErrPage::kHttpErrCodes.end()) {
////		http_code_description_ = ErrPage::kHttpErrCodes.find(http_code_)->second;
////        http_is_error_ = true;
////    } else {
////        throw BadURI();
////    }
////	FindResponseFileAddr(<#initializer#>, <#initializer#>);
//}
//
//std::string ServerResponse::FindResponseFileAddr(const Location &where,
//                                                 const std::string &filename) {
////	if (http_is_error_) {
////		const std::set<ErrPage>::iterator &err_page =
////				main_.error_pages_.find(ErrPage(http_code_));
////		if (err_page != main_.error_pages_.end()) {
////			response_filename_ = kDefaultResPath + main_.root_ + err_page->address_;
////		} else {
//////            No page for this error code
////		}
////	} else {
////		if (!main_.index_.empty()) {
////			for (std::set<std::string>::iterator indx_it = main_.index_.begin();
////				 indx_it != main_.index_.end(); ++indx_it) { // Check file like in the config
////				response_filename_ = kDefaultResPath + main_.root_ + *indx_it;
////			}
////		} else {
//////            Location has no index
////		}
////	}
//    (void) where;
//    (void) filename;
//    return "";
//}

//std::ifstream ServerResponse::TryOpenFile(const std::string &filename) {
//	std::ifstream source;
//	source.exceptions(std::ifstream::failbit);
//	try {
//		source.open(filename.c_str());
//		source.exceptions(std::ifstream::badbit);
//	} catch (const std::ifstream::failure &e) {
//		std::cout << "Requested file " + filename + " not found" << std::endl;
//	}
//	return source;
//}

//void ServerResponse::ResourceNotFound() {
//	http_is_error_ = true;
//	http_code_ = 404;
//}
//
//std::string ServerResponse::ExtractFilename(std::string &uri) {
//	return std::string();
//}

ServerResponse &ServerResponse::operator=(const ServerResponse &other) {
    if (this == &other)
        return *this;
    return *this;
}

ServerResponse::~ServerResponse() {}


//
//std::string ServerResponse::GetHeader() {
//    std::stringstream header;
//
//    header << kHttpVersion << " " << http_code_ << " " << http_code_description_ <<
//		   " " << kHttpPostfix;
//
//    return header.str();
//}

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
//std::streampos ServerResponse::GetFileSize() {
//    response_file_stream_.seekg(0, std::ios::end);
//    std::streampos filesize = response_file_stream_.tellg();
//    response_file_stream_.seekg(0, std::ios::beg);
//    return filesize;
//}
//
//void ServerResponse::SendResponse(int dest) {
//    // Check file like in the config
//    response_file_stream_.exceptions(std::ifstream::failbit);
//    response_file_stream_.open(response_filename_.c_str());
//    response_file_stream_.exceptions(std::ifstream::badbit);
//    std::streampos filesize = GetFileSize();
//    std::string header = GetHeader();
//    char buffer[kBufferSize];
//
//    //  Send HTTP header
//    if (send(dest, header.c_str(), header.size(), 0) >= 0) {
//        // Read and send the file data in chunks
//        while (filesize > 0) {
//            // Determine the number of bytes to read
//            size_t bytes_read = std::min(static_cast<size_t>(kBufferSize),
//                                         static_cast<size_t>(filesize));
//            // Read data from the file
//            response_file_stream_.read(buffer, bytes_read);
//            // Send the data over the socket
//            ssize_t bytesSent = send(dest, buffer, bytes_read, 0);
//            if (bytesSent < 0)
//                break;
//            // Update the remaining file size
//            filesize -= bytesSent;
//        }
//    }
//    //  Send header failed
//}


//
//std::string ServerResponse::ExtractParams(std::string given_uri) {
//	return std::string();
//}

//std::string ServerResponse::FindResponseFileAddr(const Location &where,
//												 const std::string &filename) {
//	if (http_is_error_) {
//		const std::set<ErrPage>::iterator &err_page =
//				main_.error_pages_.find(ErrPage(http_code_));
//		if (err_page != main_.error_pages_.end()) {
//			response_filename_ = kDefaultResPath + main_.root_ + err_page->address_;
//		} else {
////            No page for this error code
//		}
//	} else {
//		if (!main_.index_.empty()) {
//			for (std::set<std::string>::iterator indx_it = main_.index_.begin();
//				 indx_it != main_.index_.end(); ++indx_it) { // Check file like in the config
//				response_filename_ = kDefaultResPath + main_.root_ + *indx_it;
//			}
//		} else {
////            Location has no index
//		}
//	}
//}