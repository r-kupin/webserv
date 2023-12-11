//#include <gtest/gtest.h>
//#include "../../../../src/Server/request/ClientRequest.h"
//
//class RequestParsingTest : public ::testing::Test, public ClientRequest {
//public:
//    RequestParsingTest() : ClientRequest() {}
//
//protected:
//    int fd_;
//
//    virtual void TearDown() {
//        close(fd_);
//    }
//
//    void pipe_reguest_to_fd(std::string & request) {
//        int pipe_fd[2];
//        if (pipe(pipe_fd) == -1) {
//            perror("pipe");
//            FAIL();
//        }
//        if (write(pipe_fd[1], request.c_str(), request.size()) == -1) {
//            perror("write");
//            close(pipe_fd[0]);
//            close(pipe_fd[1]);
//            FAIL(); // Fail the test if write fails
//        }
//        // Close the write end of the pipe
//        close(pipe_fd[1]);
//        fd_ = pipe_fd[0];
//    }
//};
//
//TEST_F(RequestParsingTest, StandardFromFirefox) {
//    std::string request = "GET / HTTP/1.1\n\r"
//                          "Host: localhost:8080\n\r"
//                          "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0\n\r"
//                          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\n\r"
//                          "Accept-Language: en-US,en;q=0.5\n\r"
//                          "Accept-Encoding: gzip, deflate, br\n\r"
//                          "DNT: 1\n\r"
//                          "Connection: keep-alive\n\r"
//                          "Upgrade-Insecure-Requests: 1\n\r"
//                          "Sec-Fetch-Dest: document\n\r"
//                          "Sec-Fetch-Mode: navigate\n\r"
//                          "Sec-Fetch-Site: none\n\r"
//                          "Sec-Fetch-User: ?1\n\r"
//                          "";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/");
//    EXPECT_EQ(cl_req.headers_.size(), 12);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//}
//
//TEST_F(RequestParsingTest, OneParam) {
//    std::string uri = "/results?search_query=pony";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_EQ(cl_req.params_.size(), 1);
//    EXPECT_EQ(cl_req.params_["search_query"], "pony");
//}
//
//TEST_F(RequestParsingTest, MoarParamz) {
//    std::string uri = "/results?"
//                      "search_query=pony&"
//                      "search_type=pics&"
//                      "safe_search=off&"
//                      "nsfw_allow=true&"
//                      "sure=yes";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_EQ(cl_req.params_.size(), 5);
//    EXPECT_EQ(cl_req.params_["search_query"], "pony");
//    EXPECT_EQ(cl_req.params_["nsfw_allow"], "true");
//}
//
//TEST_F(RequestParsingTest, IncompleteQuerry1) {
//    std::string uri = "/results?"
//                      "search_query=";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_TRUE(cl_req.params_.empty());
//}
//
//TEST_F(RequestParsingTest, IncompleteQuerry2) {
//    std::string uri = "/results?"
//                      "search_query=pony&"
//                      "search_type=&"
//                      "safe_search=off&"
//                      "nsfw_allow=true&"
//                      "sure=yes";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_EQ(cl_req.params_.size(), 4);
//    EXPECT_EQ(cl_req.params_["search_query"], "pony");
//    EXPECT_EQ(cl_req.params_["nsfw_allow"], "true");
//    EXPECT_EQ(cl_req.params_.find("search_type"), cl_req.params_.end());
//}
//
//TEST_F(RequestParsingTest, OnlyAmpersand) {
//    std::string uri = "/results?&";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_TRUE(cl_req.params_.empty());
//}
//
//TEST_F(RequestParsingTest, OnlyAssign) {
//    std::string uri = "/results?=";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_TRUE(cl_req.params_.empty());
//}
//
//TEST_F(RequestParsingTest, AsignAndAmpersand) {
//    std::string uri = "/results?=&";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_TRUE(cl_req.params_.empty());
//}
//
//TEST_F(RequestParsingTest, AmpersandAndAsign) {
//    std::string uri = "/results?&=";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_TRUE(cl_req.params_.empty());
//}
//
//TEST_F(RequestParsingTest, QuerryWithQestionMark) {
//    std::string uri = "/results?"
//                      "search_query=pony&"
//                      "search_type=pics&"
//                      "safe_search=o?ff&"
//                      "nsfw_allow=true&"
//                      "sure=yes";
//    std::string request = "GET " + uri + " HTTP/1.1\n\r"
//                                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//    EXPECT_EQ(cl_req.method_, GET);
//    EXPECT_EQ(cl_req.address_, "/results");
//    EXPECT_EQ(cl_req.headers_.size(), 1);
//    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
//    EXPECT_EQ(cl_req.params_.size(), 5);
//    EXPECT_EQ(cl_req.params_["search_query"], "pony");
//    EXPECT_EQ(cl_req.params_["safe_search"], "o?ff");
//}
//
//TEST_F(RequestParsingTest, HTTP2) {
//    std::string request ="GET / HTTP/2.0\n\r"
//                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    EXPECT_THROW(ClientRequest badd(fd_), HTTPVersionNotSupportedException);
//}
//
//TEST_F(RequestParsingTest, PUTMETHOD) {
//    std::string request ="PUT / HTTP/2.0\n\r"
//                         "Host: localhost:8080\n\r";
//
//    pipe_reguest_to_fd(request);
//    EXPECT_THROW(ClientRequest badd(fd_), UnsupportedClientMethodException);
//}
//
//TEST_F(RequestParsingTest, EmptyRequest) {
//    std::string request = "";
//
//    pipe_reguest_to_fd(request);
//    EXPECT_ANY_THROW(ClientRequest badd(fd_));
//}
//
//TEST_F(RequestParsingTest, SomeGarbage) {
//    std::string request = "laskdijas;dlifjasd[;o'lf";
//
//    pipe_reguest_to_fd(request);
//    EXPECT_ANY_THROW(ClientRequest badd(fd_));
//}
