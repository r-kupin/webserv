#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class ExtractDataFromRequest : public ::testing::Test, public ClientRequest {
public:
    ExtractDataFromRequest() : ClientRequest() {}

protected:
        std::string req_;
};


TEST_F(ExtractDataFromRequest, ExtractUriTest) {
    req_ = "GET / HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/");

    req_ = "GET /index.html HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html");

    req_ = "GET /index.html? HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html?");

    req_ = "GET /index.html# HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html#");

    req_ = "GET /index.html?param1=value1&param2=value2 HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html?param1=value1&param2=value2");

    req_ = "GET /index.html#fragment HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html#fragment");

    req_ = "GET /index.html?# HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html?#");

    req_ = "GET /index.html?param1=value1&param2=value2# HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html?param1=value1&param2=value2#");

    req_ = "GET /index.html?param1=value1&param2=value2#fragment HTTP/1.1";
    EXPECT_EQ(ExtractUri(req_), "/index.html?param1=value1&param2=value2#fragment");
}

TEST_F(ExtractDataFromRequest, Methods) {
    req_ = "GET / HTTP/1.1";
    EXPECT_EQ(ExtractMethod(req_), GET);

    req_ = "POST / HTTP/1.1";
    EXPECT_EQ(ExtractMethod(req_), POST);

    req_ = "DELETE / HTTP/1.1";
    EXPECT_EQ(ExtractMethod(req_), DELETE);

    req_ = "PUT / HTTP/1.1";
    EXPECT_THROW(ExtractMethod(req_), UnsupportedClientMethodException);
}

TEST_F(ExtractDataFromRequest, HasQuerryTest) {
    EXPECT_FALSE(HasQuery("/"));
    EXPECT_FALSE(HasQuery("/index.html"));
    EXPECT_FALSE(HasQuery("/index.html?"));
    EXPECT_FALSE(HasQuery("/index.html#"));
    EXPECT_FALSE(HasQuery("/index.html?#"));
    EXPECT_FALSE(HasQuery("/index.html#fragment"));
    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2=value2"));
    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2=value2#"));
    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2=value2#fragment"));

    EXPECT_THROW(HasFragment("/index.html#?"), BadURI);
    EXPECT_THROW(HasFragment("/index.html#fff?"), BadURI);
}

TEST_F(ExtractDataFromRequest, HasFragment) {
    EXPECT_FALSE(HasFragment("/"));
    EXPECT_FALSE(HasFragment("/index.html"));
    EXPECT_FALSE(HasFragment("/index.html?"));
    EXPECT_FALSE(HasFragment("/index.html#"));
    EXPECT_FALSE(HasFragment("/index.html?#"));
    EXPECT_FALSE(HasFragment("/index.html?param1=value1&param2=value2"));
    EXPECT_FALSE(HasFragment("/index.html?param1=value1&param2=value2#"));
    EXPECT_TRUE(HasFragment("/index.html#fragment"));
    EXPECT_TRUE(HasFragment("/index.html?param1=value1&param2=value2#fragment"));

    EXPECT_THROW(HasFragment("/index.html#?"), BadURI);
    EXPECT_THROW(HasFragment("/index.html#fff?"), BadURI);
}

TEST_F(ExtractDataFromRequest, ExtractAddr) {
    req_ = "/";
    EXPECT_EQ(ExtractAddr(req_), "/");

    req_ = "/index.html";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html?";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html#";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html?#";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html#fragment";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html?param1=value1&param2=value2";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html?param1=value1&param2=value2#";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/index.html?param1=value1&param2=value2#fragment";
    EXPECT_EQ(ExtractAddr(req_), "/index.html");

    req_ = "/home";
    EXPECT_EQ(ExtractAddr(req_), "/home");

    req_ = "/home/index.html";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html?";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html#";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html?#";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html#fragment";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html?param1=value1&param2=value2";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html?param1=value1&param2=value2#";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");

    req_ = "/home/index.html?param1=value1&param2=value2#fragment";
    EXPECT_EQ(ExtractAddr(req_), "/home/index.html");
}

TEST_F(ExtractDataFromRequest, ExtractLast) {
    EXPECT_EQ(ExtractLastAddrStep("/"), "");
    EXPECT_EQ(ExtractLastAddrStep("/index.html"), "index.html");
    EXPECT_EQ(ExtractLastAddrStep("/home/index.html"), "index.html");
    EXPECT_EQ(ExtractLastAddrStep("example.com/path/to/resource"), "resource");

//    EXPECT_EQ(ExtractLastAddrStep("example.com/path/to/resource/"), "resource");
}

TEST_F(ExtractDataFromRequest, HasBodyNo) {
    v_str req(15);
    req[0] =  "GET / HTTP/1.1";
    req[1] =  "Host: localhost:8080";
    req[2] =  "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0 Gecko/20100101 Firefox/115.0";
    req[3] =  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8";
    req[4] =  "Accept-Language: en-US,en;q=0.5";
    req[5] =  "Accept-Encoding: gzip, deflate, br";
    req[6] =  "DNT: 1";
    req[7] =  "Connection: keep-alive";
    req[8] =  "Upgrade-Insecure-Requests: 1";
    req[9] =  "Sec-Fetch-Dest: document";
    req[10] =  "Sec-Fetch-Mode: navigate";
    req[11] =  "Sec-Fetch-Site: none";
    req[12] =  "Sec-Fetch-User: ?1";
    req[13] =  "";
    EXPECT_FALSE(HasBody(req));
}

TEST_F(ExtractDataFromRequest, HasBodyYes) {
    v_str req(6);
    req[0] = "POST /test HTTP/1.1";
    req[1] = "Host: foo.example";
    req[2] = "Content-Type: application/x-www-form-urlencoded";
    req[3] = "Content-Length: 27";
    req[4] = "";
    req[5] = "field1=value1&field2=value2";
    EXPECT_TRUE(HasBody(req));
}

TEST_F(ExtractDataFromRequest, FillUriParams) {
    FillUriParams("/index.html?param1=value1&param2=value2");
    EXPECT_EQ(params_.size(), 2);
    EXPECT_EQ(params_["param1"], "value1");
    EXPECT_EQ(params_["param2"], "value2");
}

TEST_F(ExtractDataFromRequest, FillUriParamsWithFragment) {
    FillUriParams("/index.html?param1=value1&param2=value2#fragment");
    EXPECT_EQ(params_.size(), 2);
    EXPECT_EQ(params_["param1"], "value1");
    EXPECT_EQ(params_["param2"], "value2");
}

TEST_F(ExtractDataFromRequest, FillUriParamsIncompletePair) {
    FillUriParams("/index.html?param1=value1&param2#fragment");
    EXPECT_EQ(params_.size(), 1);
    EXPECT_EQ(params_["param1"], "value1");
}

//todo how to handle?
//TEST_F(ExtractDataFromRequest, FillUriParamsNoAmpersand) {
//    FillUriParams("/index.html?param1=value1param2=value2#fragment");
//    EXPECT_EQ(params_.size(), 1);
//}
