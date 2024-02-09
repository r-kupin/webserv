#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class ParsedRequestLevelTest : public ::testing::Test, public ClientRequest {
public:
    ParsedRequestLevelTest() : ClientRequest() {}
protected:
    v_str firefox_GET_req_;
    v_str example_POST_req_;
    v_str no_headers_POST_req_;
    v_str multiline_body_;
    v_str req_;

    void setup_ff_Get() {
        firefox_GET_req_.emplace_back("GET / HTTP/1.1");
        firefox_GET_req_.emplace_back("Host: localhost:8080");
        firefox_GET_req_.emplace_back(
                "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0 "
                "Gecko/20100101 Firefox/115.0");
        firefox_GET_req_.emplace_back(
                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
                "image/avif,image/webp,*/*;q=0.8");
        firefox_GET_req_.emplace_back("Accept-Language: en-US,en;q=0.5");
        firefox_GET_req_.emplace_back("Accept-Encoding: gzip, deflate, br");
        firefox_GET_req_.emplace_back("DNT: 1");
        firefox_GET_req_.emplace_back("Connection: keep-alive");
        firefox_GET_req_.emplace_back("Upgrade-Insecure-Requests: 1");
        firefox_GET_req_.emplace_back("Sec-Fetch-Dest: document");
        firefox_GET_req_.emplace_back("Sec-Fetch-Mode: navigate");
        firefox_GET_req_.emplace_back("Sec-Fetch-Site: none");
        firefox_GET_req_.emplace_back("Sec-Fetch-User: ?1");
        firefox_GET_req_.emplace_back("");
    }

    void setup_example_POST() {
        example_POST_req_.emplace_back("POST /test HTTP/1.1");
        example_POST_req_.emplace_back("Host: foo.example");
        example_POST_req_.emplace_back(
                "Content-Type: application/x-www-form-urlencoded");
        example_POST_req_.emplace_back("Content-Length: 27");
    }

    void setup_no_headers_POST() {
        no_headers_POST_req_.emplace_back("POST /test HTTP/1.1");
    }

    virtual void SetUp() {
         setup_ff_Get();
         setup_example_POST();
         setup_no_headers_POST();
    }
};

TEST_F(ParsedRequestLevelTest, CheckRequestWrongHTTPVersion) {
    req_.emplace_back("GET / HTTP/1.0");
    EXPECT_THROW(CheckRequest(req_), HTTPVersionNotSupportedException);
}

TEST_F(ParsedRequestLevelTest, CheckRequestWrongMethod) {
    req_.emplace_back("OPTIONS / HTTP/1.1");
    EXPECT_THROW(CheckRequest(req_), UnsupportedClientMethodException);
}

TEST_F(ParsedRequestLevelTest, HasHeadersTest) {
    EXPECT_TRUE(HasHeaders(firefox_GET_req_));
    EXPECT_TRUE(HasHeaders(example_POST_req_));
    EXPECT_FALSE(HasHeaders(no_headers_POST_req_));
}

TEST_F(ParsedRequestLevelTest, ExtractHeaders_HasHeadersNoBody) {
    FillHeaders(firefox_GET_req_);
    EXPECT_EQ(GetHeaders().size(), 12);
    EXPECT_EQ(GetHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(GetHeaders().at("User-Agent"), "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; "
                                             "rv:109.0 Gecko/20100101 Firefox/115.0");
    EXPECT_EQ(GetHeaders().at("Accept"), "text/html,application/xhtml+xml,application/xml;"
                                         "q=0.9,image/avif,image/webp,*/*;"
                                         "q=0.8");
    EXPECT_EQ(GetHeaders().at("Accept-Language"), "en-US,en;q=0.5");
    EXPECT_EQ(GetHeaders().at("Accept-Encoding"), "gzip, deflate, br");
    EXPECT_EQ(GetHeaders().at("DNT"), "1");
    EXPECT_EQ(GetHeaders().at("Connection"), "keep-alive");
    EXPECT_EQ(GetHeaders().at("Upgrade-Insecure-Requests"), "1");
    EXPECT_EQ(GetHeaders().at("Sec-Fetch-Dest"), "document");
    EXPECT_EQ(GetHeaders().at("Sec-Fetch-Mode"), "navigate");
    EXPECT_EQ(GetHeaders().at("Sec-Fetch-Site"), "none");
    EXPECT_EQ(GetHeaders().at("Sec-Fetch-User"), "?1");
}

TEST_F(ParsedRequestLevelTest, ExtractHeaders_HasBodyAfterHeaders) {
    FillHeaders(example_POST_req_);
    EXPECT_EQ(GetHeaders().size(), 3);
    EXPECT_EQ(GetHeaders().at("Host"), "foo.example");
    EXPECT_EQ(GetHeaders().at("Content-Type"),
              "application/x-www-form-urlencoded");
    EXPECT_EQ(GetHeaders().at("Content-Length"), "27");
}
