#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class URLLevelTest : public ::testing::Test, public ClientRequest {
public:
    URLLevelTest() : ClientRequest() {}

protected:
    std::string req_;
};

TEST_F(URLLevelTest, CheckURL) {
    method_ = GET;
    EXPECT_NO_THROW(CheckURL("/"));
    EXPECT_NO_THROW(CheckURL("/index.html"));
    EXPECT_NO_THROW(CheckURL("/index.html?"));
    EXPECT_NO_THROW(CheckURL("/index.html#"));
    EXPECT_NO_THROW(CheckURL("/index.html#fragment"));
    EXPECT_NO_THROW(CheckURL("/index.html?#fragment"));
    EXPECT_NO_THROW(CheckURL("/index.html?param1#fragment"));
    EXPECT_NO_THROW(CheckURL("/index.html?param1=#fragment"));
    EXPECT_NO_THROW(CheckURL("/index.html?=value1#fragment"));
    EXPECT_NO_THROW(CheckURL("/index.html?#"));
    EXPECT_NO_THROW(CheckURL("/index.html?param1=value1&param2=value2"));
    EXPECT_NO_THROW(CheckURL("/index.html?param1=value1&param2=value2#"));
    EXPECT_NO_THROW(CheckURL("/index.html?param1=value1&param2=value2#fragment"));

    EXPECT_THROW(CheckURL(""), BadURL);
    EXPECT_THROW(CheckURL("/index.html#?"), BadURL);
    EXPECT_THROW(CheckURL("/index.html#fff?"), BadURL);

    method_ = POST;
    EXPECT_THROW(CheckURL("/index.html#fragment"),
                 BadRequestException);
    EXPECT_THROW(CheckURL("/index.html?param1=value1&param2=value2"),
                 BadRequestException);
    EXPECT_THROW(CheckURL("/index.html?param1=value1&param2=value2#fragment"),
                 BadRequestException);
}

TEST_F(URLLevelTest, HasQuerryTest) {
    EXPECT_FALSE(HasQuery("/"));
    EXPECT_FALSE(HasQuery("/index.html"));
    EXPECT_FALSE(HasQuery("/index.html?"));
    EXPECT_FALSE(HasQuery("/index.html#"));
    EXPECT_FALSE(HasQuery("/index.html#fragment"));
    EXPECT_FALSE(HasQuery("/index.html?#fragment"));
    EXPECT_FALSE(HasQuery("/index.html?param1#fragment"));
    EXPECT_FALSE(HasQuery("/index.html?param1=#fragment"));
    EXPECT_FALSE(HasQuery("/index.html?=value1#fragment"));
    EXPECT_FALSE(HasQuery("/index.html?#"));

    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2="));
    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2=value2"));
    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2=value2#"));
    EXPECT_TRUE(HasQuery("/index.html?param1=value1&param2=value2#fragment"));

    EXPECT_THROW(HasQuery("/index.html#?"), BadURL);
    EXPECT_THROW(HasQuery("/index.html#fff?"), BadURL);
}

TEST_F(URLLevelTest, HasFragment) {
    EXPECT_FALSE(HasFragment("/"));
    EXPECT_FALSE(HasFragment("/index.html"));
    EXPECT_FALSE(HasFragment("/index.html?"));
    EXPECT_FALSE(HasFragment("/index.html#"));
    EXPECT_FALSE(HasFragment("/index.html?#"));
    EXPECT_FALSE(HasFragment("/index.html?param1=value1&param2=value2"));
    EXPECT_FALSE(HasFragment("/index.html?param1=value1&param2=value2#"));

    EXPECT_TRUE(HasFragment("/index.html#fragment"));
    EXPECT_TRUE(HasFragment("/index.html?param1=value1&param2=value2#fragment"));

    EXPECT_THROW(HasFragment("/index.html#?"), BadURL);
    EXPECT_THROW(HasFragment("/index.html#fff?"), BadURL);
}

TEST_F(URLLevelTest, ExtractAddr) {
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


TEST_F(URLLevelTest, ExtractQuerryTest) {
    EXPECT_EQ(ExtractQuerry("/"), "");
    EXPECT_EQ(ExtractQuerry("/index.html"), "");
    EXPECT_EQ(ExtractQuerry("/index.html?"), "");
    EXPECT_EQ(ExtractQuerry("/index.html#"), "");
    EXPECT_EQ(ExtractQuerry("/index.html#fragment"), "");
    EXPECT_EQ(ExtractQuerry("/index.html?#fragment"), "");
    EXPECT_EQ(ExtractQuerry("/index.html?param1#fragment"), "");
    EXPECT_EQ(ExtractQuerry("/index.html?param1=#fragment"), "");
    EXPECT_EQ(ExtractQuerry("/index.html?=value1#fragment"), "");
    EXPECT_EQ(ExtractQuerry("/index.html?#"), "");

    EXPECT_EQ(ExtractQuerry("/index.html?param1=value1&param2="),
                            "param1=value1&param2=");
    EXPECT_EQ(ExtractQuerry("/index.html?param1=value1&param2=value2"),
                            "param1=value1&param2=value2");
    EXPECT_EQ(ExtractQuerry("/index.html?param1=value1&param2=value2#"),
                            "param1=value1&param2=value2");
    EXPECT_EQ(ExtractQuerry("/index.html?param1=value1&param2=value2#fragment"),
                            "param1=value1&param2=value2");
}

TEST_F(URLLevelTest, FillUrlParams) {
    FillUrlParams("/index.html?param1=value1&param2=value2");
    EXPECT_EQ(params_.size(), 2);
    EXPECT_EQ(params_["param1"], "value1");
    EXPECT_EQ(params_["param2"], "value2");
}

TEST_F(URLLevelTest, FillUriParamsWithFragment) {
    FillUrlParams("/index.html?param1=value1&param2=value2#fragment");
    EXPECT_EQ(params_.size(), 2);
    EXPECT_EQ(params_["param1"], "value1");
    EXPECT_EQ(params_["param2"], "value2");
}

TEST_F(URLLevelTest, FillUriParamsIncompletePair) {
    FillUrlParams("/index.html?param1=value1&param2#fragment");
    EXPECT_EQ(params_.size(), 1);
    EXPECT_EQ(params_["param1"], "value1");
}

TEST_F(URLLevelTest, FillUriParamsIncompletePair1) {
    FillUrlParams("/index.html?param1=value1&param2=#fragment");
    EXPECT_EQ(params_.size(), 1);
    EXPECT_EQ(params_["param1"], "value1");
}


TEST_F(URLLevelTest, FillUriParamsIncompletePair2) {
    FillUrlParams("/index.html?param1=value1&=param2#fragment");
    EXPECT_EQ(params_.size(), 1);
    EXPECT_EQ(params_["param1"], "value1");
}

TEST_F(URLLevelTest, FillUriParamsIncompletePair3) {
    FillUrlParams("/index.html?param1=value1&=param2=#fragment");
    EXPECT_EQ(params_.size(), 1);
    EXPECT_EQ(params_["param1"], "value1");
}

TEST_F(URLLevelTest, FillUriParamsIncompletePair4) {
    FillUrlParams("/index.html?param1=value1=&=param2=#fragment");
    EXPECT_EQ(params_.size(), 1);
    EXPECT_EQ(params_["param1"], "value1=");
}

// todo not sure
//TEST_F(URLLevelTest, FillUriMultipleQuestionMarks) {
//    FillUrlParams("/results?"
//                      "param1=value1&"
//                      "param2=va?ue2&"
//                      "param3=value3");
//    EXPECT_EQ(params_.size(), 3);
//    EXPECT_EQ(params_["param1"], "value1");
//    EXPECT_EQ(params_["param2"], "va?ue2");
//    EXPECT_EQ(params_["param3"], "value3");
//}
//
//// todo not sure
//TEST_F(URLLevelTest, FillUriMultipleQuestionMarksBeginning) {
//    FillUrlParams("/results??"
//                      "param1=value1&"
//                      "param2=va?ue2&"
//                      "param3=value3");
//    EXPECT_EQ(params_.size(), 3);
//    EXPECT_EQ(params_["param1"], "value1");
//    EXPECT_EQ(params_["param2"], "va?ue2");
//    EXPECT_EQ(params_["param3"], "value3");
//}
//
//// todo not sure
//TEST_F(URLLevelTest, FillUriMultipleAmpsMarks) {
//    FillUrlParams("/results?"
//                      "param1=value1&&&&"
//                      "param2=value2&&&"
//                      "param3=value3");
//    EXPECT_EQ(params_.size(), 3);
//    EXPECT_EQ(params_["param1"], "value1");
//    EXPECT_EQ(params_["param2"], "va?ue2");
//    EXPECT_EQ(params_["param3"], "value3");
//}
//
//// todo not sure
//TEST_F(URLLevelTest, AmpInTheBeginning) {
//    FillUrlParams("/results?&"
//                      "param1=value1&"
//                      "param2=value2&"
//                      "param3=value3");
//    EXPECT_EQ(params_.size(), 3);
//    EXPECT_EQ(params_["param1"], "value1");
//    EXPECT_EQ(params_["param2"], "va?ue2");
//    EXPECT_EQ(params_["param3"], "value3");
//}
//
//// todo not sure
//TEST_F(URLLevelTest, AmpInTheEnd) {
//    FillUrlParams("/results?&"
//                      "param1=value1&"
//                      "param2=value2&"
//                      "param3=value3");
//    EXPECT_EQ(params_.size(), 3);
//    EXPECT_EQ(params_["param1"], "value1");
//    EXPECT_EQ(params_["param2"], "va?ue2");
//    EXPECT_EQ(params_["param3"], "value3");
//}

TEST_F(URLLevelTest, ExtractFragment) {
    EXPECT_EQ(ExtractFragment("/index.html#fragment"), "fragment");
    EXPECT_EQ(ExtractFragment("/index.html?param1=value1&param2=value2#fragment"), "fragment");
}

TEST_F(URLLevelTest, ExtractLast) {
    EXPECT_EQ(ExtractLastAddrStep("/"), "/");
    EXPECT_EQ(ExtractLastAddrStep("/index.html"), "/index.html");
    EXPECT_EQ(ExtractLastAddrStep("/home/index.html"), "/index.html");
    EXPECT_EQ(ExtractLastAddrStep("example.com/path/to/resource"), "/resource");

//    EXPECT_EQ(ExtractLastAddrStep("example.com/path/to/resource / "), "resource");
}

//todo how to handle?
//TEST_F(ExtractDataFromRequest, FillUriParamsNoAmpersand) {
//    FillUriParams("/index.html?param1=value1param2=value2#fragment");
//    EXPECT_EQ(params_.size(), 1);
//}
