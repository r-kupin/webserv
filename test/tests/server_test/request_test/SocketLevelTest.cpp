#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class SocketLevelTest : public ::testing::Test, public ClientRequest {
public:
    SocketLevelTest() : ClientRequest() {}

protected:
    std::string firefox_GET_req_;
    std::string example_POST_req_;
    std::string no_headers_POST_req_;

    int fd_;

    virtual void TearDown() {
        close(fd_);
    }

    void pipe_reguest_to_fd(std::string & request) {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("pipe");
            FAIL();
        }
        if (write(pipe_fd[1], request.c_str(), request.size()) == -1) {
            perror("write");
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            FAIL(); // Fail the test if write fails
        }
        // Close the write end of the pipe
        close(pipe_fd[1]);
        fd_ = pipe_fd[0];
    }

    virtual void SetUp() {
        firefox_GET_req_ = "GET / HTTP/1.1\n\r"
                          "Host: localhost:8080\n\r"
                          "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0\n\r"
                          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\n\r"
                          "Accept-Language: en-US,en;q=0.5\n\r"
                          "Accept-Encoding: gzip, deflate, br\n\r"
                          "DNT: 1\n\r"
                          "Connection: keep-alive\n\r"
                          "Upgrade-Insecure-Requests: 1\n\r"
                          "Sec-Fetch-Dest: document\n\r"
                          "Sec-Fetch-Mode: navigate\n\r"
                          "Sec-Fetch-Site: none\n\r"
                          "Sec-Fetch-User: ?1\n\r"
                          "\n\r";
        example_POST_req_ = "POST /test HTTP/1.1\n\r"
                            "Host: foo.example\n\r"
                            "Content-Type: application/x-www-form-urlencoded\n\r"
                            "Content-Length: 27\n\r"
                            "\n\r"
                            "field1=value1&field2=value2\n\r";
        no_headers_POST_req_ = "POST /test HTTP/1.1\n\r"
                               "\n\r"
                               "field1=value1&field2=value2\n\r";
    }
};

class ReadFromSocketTest : public SocketLevelTest {};

TEST_F(ReadFromSocketTest, ReadFromFD_FFGetRequest) {
    pipe_reguest_to_fd(firefox_GET_req_);
    v_str req = ReadFromSocket(fd_);
    EXPECT_EQ(req.size(), 14);
    EXPECT_EQ(req[0], "GET / HTTP/1.1");
    EXPECT_EQ(req[1], "Host: localhost:8080");
    EXPECT_EQ(req[2], "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0");
    EXPECT_EQ(req[3], "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
    EXPECT_EQ(req[4], "Accept-Language: en-US,en;q=0.5");
    EXPECT_EQ(req[5], "Accept-Encoding: gzip, deflate, br");
    EXPECT_EQ(req[6], "DNT: 1");
    EXPECT_EQ(req[7], "Connection: keep-alive");
    EXPECT_EQ(req[8], "Upgrade-Insecure-Requests: 1");
    EXPECT_EQ(req[9], "Sec-Fetch-Dest: document");
    EXPECT_EQ(req[10], "Sec-Fetch-Mode: navigate");
    EXPECT_EQ(req[11], "Sec-Fetch-Site: none");
    EXPECT_EQ(req[12], "Sec-Fetch-User: ?1");
    EXPECT_EQ(req[13], "");
}

TEST_F(ReadFromSocketTest, ReadFromFD_CurlPostRequest) {
    pipe_reguest_to_fd(example_POST_req_);
    v_str req = ReadFromSocket(fd_);
    EXPECT_EQ(req.size(), 6);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
    EXPECT_EQ(req[1],"Host: foo.example");
    EXPECT_EQ(req[2],"Content-Type: application/x-www-form-urlencoded");
    EXPECT_EQ(req[3],"Content-Length: 27");
    EXPECT_EQ(req[4],"");
    EXPECT_EQ(req[5],"field1=value1&field2=value2");
}

TEST_F(ReadFromSocketTest, ReadFromFD_NoHeadersPostRequest) {
    pipe_reguest_to_fd(no_headers_POST_req_);
    v_str req = ReadFromSocket(fd_);
    EXPECT_EQ(req.size(), 3);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
    EXPECT_EQ(req[1],"");
    EXPECT_EQ(req[2],"field1=value1&field2=value2");
}

TEST_F(ReadFromSocketTest, ReadFromFD_Empty) {
    std::string request = "";

    pipe_reguest_to_fd(request);
    EXPECT_THROW(ReadFromSocket(fd_), ReadFromSocketFailedException);
}

class InitTest : public SocketLevelTest {};

TEST_F(InitTest, StandardGetFromFirefox) {
    pipe_reguest_to_fd(firefox_GET_req_);
    Init(fd_);
    EXPECT_EQ(method_, GET);
    EXPECT_EQ(addr_, "/");
    EXPECT_EQ(addr_last_step_, "/");
    EXPECT_EQ(body_, "");
    EXPECT_EQ(fragment_, "");
    EXPECT_EQ(params_.size(), 0);
    EXPECT_EQ(headers_.size(), 12);
    EXPECT_EQ(headers_["Host"], "localhost:8080");
    EXPECT_EQ(headers_["User-Agent"], "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0");
    EXPECT_EQ(headers_["Accept"], "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
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

TEST_F(InitTest, example_POST_req_) {
    pipe_reguest_to_fd(example_POST_req_);
    Init(fd_);
    EXPECT_EQ(method_, POST);
    EXPECT_EQ(addr_, "/test");
    EXPECT_EQ(addr_last_step_, "/test");
    EXPECT_EQ(body_, "field1=value1&field2=value2");
    EXPECT_EQ(fragment_, "");
    EXPECT_EQ(params_.size(), 0);
    EXPECT_EQ(headers_.size(), 3);
    EXPECT_EQ(headers_["Host"], "foo.example");
    EXPECT_EQ(headers_["Content-Type"], "application/x-www-form-urlencoded");
    EXPECT_EQ(headers_["Content-Length"], "27");
}

TEST_F(InitTest, no_headers_POST_req_) {
    pipe_reguest_to_fd(no_headers_POST_req_);
    Init(fd_);
    EXPECT_EQ(method_, POST);
    EXPECT_EQ(addr_, "/test");
    EXPECT_EQ(addr_last_step_, "/test");
    EXPECT_EQ(body_, "field1=value1&field2=value2");
    EXPECT_EQ(fragment_, "");
    EXPECT_EQ(params_.size(), 0);
    EXPECT_EQ(headers_.size(), 0);
}

TEST_F(InitTest, ParamsNFragment) {
    std::string addr = "/results";
    std::string querry = "?search_query=pony&"
                         "search_type=pics&"
                         "safe_search=off&"
                         "nsfw_allow=true&"
                         "sure=yes";
    std::string fragment = "#34";
    std::string uri = addr + querry + fragment;
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
                                         "Host: localhost:8080\n\r"
                                         "\n\r";
    pipe_reguest_to_fd(request);
    Init(fd_);
    EXPECT_EQ(method_, GET);
    EXPECT_EQ(addr_, "/results");
    EXPECT_EQ(addr_last_step_, "/results");
    EXPECT_EQ(body_, "");
    EXPECT_EQ(fragment_, "34");
    EXPECT_EQ(headers_.size(), 1);
    EXPECT_EQ(headers_["Host"], "localhost:8080");
    EXPECT_EQ(params_.size(), 5);
    EXPECT_EQ(params_["search_query"], "pony");
    EXPECT_EQ(params_["search_type"], "pics");
    EXPECT_EQ(params_["safe_search"], "off");
    EXPECT_EQ(params_["nsfw_allow"], "true");
    EXPECT_EQ(params_["sure"], "yes");
}

TEST_F(InitTest, ParamsNFragmentArgIncomplete) {
    std::string addr = "/results";
    std::string querry = "?search_query=pony&"
                         "search_type=pics&"
                         "safe_search=off&"
                         "nsfw_allow=true&"
                         "sure=";
    std::string fragment = "#34";
    std::string uri = addr + querry + fragment;
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
                                         "Host: localhost:8080\n\r"
                                         "\n\r";
    pipe_reguest_to_fd(request);
    Init(fd_);
    EXPECT_EQ(method_, GET);
    EXPECT_EQ(addr_, "/results");
    EXPECT_EQ(addr_last_step_, "/results");
    EXPECT_EQ(body_, "");
    EXPECT_EQ(fragment_, "34");
    EXPECT_EQ(headers_.size(), 1);
    EXPECT_EQ(headers_["Host"], "localhost:8080");
    EXPECT_EQ(params_.size(), 4);
    EXPECT_EQ(params_["search_query"], "pony");
    EXPECT_EQ(params_["search_type"], "pics");
    EXPECT_EQ(params_["safe_search"], "off");
    EXPECT_EQ(params_["nsfw_allow"], "true");
}
