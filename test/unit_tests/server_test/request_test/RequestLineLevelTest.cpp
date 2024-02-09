#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class RequestLineLevelTest : public ::testing::Test, public ClientRequest {
public:
    RequestLineLevelTest() : ClientRequest() {}

protected:
    std::string req_;
};

TEST_F(RequestLineLevelTest, Methods) {
    req_ = "GET / HTTP/1.1";
    EXPECT_EQ(ExtractMethod(req_), GET);

    req_ = "POST / HTTP/1.1";
    EXPECT_EQ(ExtractMethod(req_), POST);

    req_ = "DELETE / HTTP/1.1";
    EXPECT_EQ(ExtractMethod(req_), DELETE);

    req_ = "PUT / HTTP/1.1";
    EXPECT_THROW(ExtractMethod(req_), UnsupportedClientMethodException);
}

TEST_F(RequestLineLevelTest, ExtractUrlTest) {
    req_ = "GET / HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/");

    req_ = "GET /index.html HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html");

    req_ = "GET /index.html? HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html?");

    req_ = "GET /index.html# HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html#");

    req_ = "GET /index.html?param1=value1&param2=value2 HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html?param1=value1&param2=value2");

    req_ = "GET /index.html#fragment HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html#fragment");

    req_ = "GET /index.html?# HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html?#");

    req_ = "GET /index.html?#fragment HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html?#fragment");

    req_ = "GET /index.html?param1=value1&param2=value2# HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html?param1=value1&param2=value2#");

    req_ = "GET /index.html?param1=value1&param2=value2#fragment HTTP/1.1";
    EXPECT_EQ(ExtractUrl(req_), "/index.html?param1=value1&param2=value2#fragment");
}
