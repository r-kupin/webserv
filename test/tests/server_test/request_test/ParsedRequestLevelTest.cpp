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
        example_POST_req_.emplace_back("");
        example_POST_req_.emplace_back("field1=value1&field2=value2");
    }

    void setup_no_headers_POST() {
        no_headers_POST_req_.emplace_back("POST /test HTTP/1.1");
        no_headers_POST_req_.emplace_back("");
        no_headers_POST_req_.emplace_back("field1=value1&field2=value2");
    }

    virtual void SetUp() {
            setup_ff_Get();
            setup_example_POST();
            setup_no_headers_POST();
    }

};

TEST_F(ParsedRequestLevelTest, CheckRequestWrongHTTPVersion) {
    req_.emplace_back("GET / HTTP/1.0");
    EXPECT_THROW(CheckRequest(req_), BadRequestException);
}

TEST_F(ParsedRequestLevelTest, CheckRequestWrongMethod) {
    req_.emplace_back("OPTIONS / HTTP/1.1");
    EXPECT_THROW(CheckRequest(req_), UnsupportedClientMethodException);
}

TEST_F(ParsedRequestLevelTest, CheckRequestPOSTHasHeadersNoBody) {
    req_.emplace_back("POST / HTTP/1.1");
    req_.emplace_back("Host: localhost:8080");
    req_.emplace_back("");
    EXPECT_THROW(CheckRequest(req_), BadRequestException);
}

TEST_F(ParsedRequestLevelTest, CheckRequestPOSTHasNoHeadersNoBody) {
    req_.emplace_back("POST / HTTP/1.1");
    req_.emplace_back("");
    EXPECT_THROW(CheckRequest(req_), BadRequestException);
}

TEST_F(ParsedRequestLevelTest, CheckRequestGETHasHeadersAndBody) {
    firefox_GET_req_.emplace_back("field1=value1&field2=value2");
    EXPECT_THROW(CheckRequest(firefox_GET_req_), BadRequestException);
}

TEST_F(ParsedRequestLevelTest, CheckRequestTest) {
    EXPECT_NO_THROW(CheckRequest(firefox_GET_req_));
    EXPECT_NO_THROW(CheckRequest(example_POST_req_));
    EXPECT_NO_THROW(CheckRequest(no_headers_POST_req_));
}

TEST_F(ParsedRequestLevelTest, HasHeadersTest) {
    EXPECT_TRUE(HasHeaders(firefox_GET_req_));
    EXPECT_TRUE(HasHeaders(example_POST_req_));
    EXPECT_FALSE(HasHeaders(no_headers_POST_req_));
}

TEST_F(ParsedRequestLevelTest, HasBodyTest) {
    EXPECT_FALSE(HasBody(firefox_GET_req_));
    EXPECT_TRUE(HasBody(example_POST_req_));
    EXPECT_TRUE(HasBody(no_headers_POST_req_));

}

TEST_F(ParsedRequestLevelTest, ExtractHeaders_HasHeadersNoBody) {
    FillHeaders(firefox_GET_req_);
    EXPECT_EQ(headers_.size(), 12);
    EXPECT_EQ(headers_["Host"], "localhost:8080");
    EXPECT_EQ(headers_["User-Agent"], "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0 "
                   "Gecko/20100101 Firefox/115.0");
    EXPECT_EQ(headers_["Accept"], "text/html,application/xhtml+xml,application/xml;q=0.9,"
                      "image/avif,image/webp,*/*;q=0.8");
    EXPECT_EQ(headers_["Accept-Language"], "en-US,en;q=0.5");
    EXPECT_EQ(headers_["Accept-Encoding"], "gzip, deflate, br");
    EXPECT_EQ(headers_["DNT"], "1");
    EXPECT_EQ(headers_["Connection"], "keep-alive");
    EXPECT_EQ(headers_["Upgrade-Insecure-Requests"], "1");
    EXPECT_EQ(headers_["Sec-Fetch-Dest"], "document");
    EXPECT_EQ(headers_["Sec-Fetch-Mode"], "navigate");
    EXPECT_EQ(headers_["Sec-Fetch-Site"], "none");
    EXPECT_EQ(headers_["Sec-Fetch-User"], "?1");
}

TEST_F(ParsedRequestLevelTest, ExtractHeaders_HasBodyAfterHeaders) {
    FillHeaders(example_POST_req_);
    EXPECT_EQ(headers_.size(), 3);
    EXPECT_EQ(headers_["Host"], "foo.example");
    EXPECT_EQ(headers_["Content-Type"], "application/x-www-form-urlencoded");
    EXPECT_EQ(headers_["Content-Length"], "27");
}

TEST_F(ParsedRequestLevelTest, ExtractBodyTest) {
    EXPECT_EQ(ExtractBody(example_POST_req_), "field1=value1&field2=value2");
    EXPECT_EQ(ExtractBody(no_headers_POST_req_), "field1=value1&field2=value2");
}
