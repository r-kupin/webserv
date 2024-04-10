#include <cstdlib>
#include "Location.h"

bool is_number(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

bool is_internal_address(const std::string &str) {
    return str.at(0) == '/';
}

bool is_external_address(const std::string &str) {
    std::string address_prefix = "http";
    return str.substr(0, address_prefix.size()) == address_prefix;
}

bool is_address(const std::string &str) {
    return is_internal_address(str) || is_external_address(str);
}

/**
 * from nginx docs:
 *  	return code [text];
 *  	return code URL;
 *  	return URL;
 *  Stops processing and returns the specified code to a client.
 *  Starting from version 0.8.42, it is possible to specify either a redirect
 * URL (for codes 301, 302, 303, 307, and 308) or the response body text (for
 * other codes). A response body text and redirect URL can contain variables.
 * As a special case, a redirect URL can be specified as a URI local to this
 * server, in which case the full redirect URL is formed according to the
 * request scheme ($scheme) and the server_name_in_redirect and
 * port_in_redirect directives.
 *  In addition, a URL for temporary redirect with the code 302 can be
 * specified as the sole parameter. Such a parameter should start with the
 * “http://”, “https://”, or “$scheme” string. A URL can contain variables.
 *
 *  If return has 1 argument, it is should be a return code or address.
 *  If return has 2 arguments, it is should be a return code and address or
 * custom message !! depending on the return code !!
 *  There can't be more than 2 args, and code can't be the second arg.
 *  The redirect, if only address specified, is done with 302 code
 */
void Location::HandleLocationReturn(const v_str &directives_) {
    if (directives_.size() == 2 &&
            (is_number(directives_[1]) || is_address(directives_[1]))) {
        Handle1ArgReturn(directives_);
    } else if (directives_.size() == 3 && is_number(directives_[1])) {
        Handle2ArgReturn(directives_);
    } else {
        ThrowLocationException("Return directive is wrong");
    }
}

void Location::Handle2ArgReturn(const v_str &directives_) {
    HandleCode(directives_[1]);
    if (is_address(directives_[2])) {
        if (!Utils::Get().IsRedirectCode(return_code_))
            ThrowLocationException("Return directive is wrong");
        HandleAddress(directives_[2]);
    } else {
        if (!Utils::Get().IsRedirectCode(return_code_))
            ThrowLocationException("Return directive is wrong");
        return_custom_message_ = directives_[2];
    }
}

void Location::Handle1ArgReturn(const v_str &directives_) {
    if (is_address(directives_[1])) {
        HandleAddress(directives_[1]);
        return_code_ = 302;
    } else if (is_number(directives_[1])) {
        HandleCode(directives_[1]);
    } else {
        ThrowLocationException("Return directive is wrong");
    }
}

void Location::HandleCode(const std::string &str) {
    if (is_number(str)) {
        int code = atoi(str.c_str());
        if (!Utils::Get().IsValidHTTPCode(code)) {
            ThrowLocationException("Return code is wrong");
        }
        return_code_ = code;
    }
}

void Location::HandleAddress(const std::string &str) {
    if (is_address(str)) {
        if (return_internal_address_.empty() && return_external_address_.empty()) {
            if (is_internal_address(str)) {
                return_internal_address_ = str;
            } else if (is_external_address(str)) {
                return_external_address_ = str;
            }
        } else {
            ThrowLocationException("Return address is already defined or "
                                   "misconfigured");
        }
    }
}

