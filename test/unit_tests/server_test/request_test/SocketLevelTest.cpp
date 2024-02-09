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
    v_char body_;

    virtual void TearDown() {
        close(fd_);
    }

    void pipe_reguest_to_fd(std::string request) {
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
        firefox_GET_req_ = "GET / HTTP/1.1\r\n"
                          "Host: localhost:8080\r\n"
                          "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0\r\n"
                          "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
                          "Accept-Language: en-US,en;q=0.5\r\n"
                          "Accept-Encoding: gzip, deflate, br\r\n"
                          "DNT: 1\r\n"
                          "Connection: keep-alive\r\n"
                          "Upgrade-Insecure-Requests: 1\r\n"
                          "Sec-Fetch-Dest: document\r\n"
                          "Sec-Fetch-Mode: navigate\r\n"
                          "Sec-Fetch-Site: none\r\n"
                          "Sec-Fetch-User: ?1\r\n\r\n";

        example_POST_req_ = "POST /test HTTP/1.1\r\n"
                            "Host: foo.example\r\n"
                            "Content-Type: application/x-www-form-urlencoded\r\n";

        no_headers_POST_req_ = "POST /test HTTP/1.1\r\n\r\n";
    }
};

class ReadFromSocketTest : public SocketLevelTest {};

TEST_F(ReadFromSocketTest, ReadFromFD_FFGetRequest) {
    pipe_reguest_to_fd(firefox_GET_req_);
    v_str req = ReadFromSocket(fd_, BUFFER_SIZE);
    EXPECT_EQ(req.size(), 13);
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
    EXPECT_TRUE(SocketLevelTest::body_.empty());
}

TEST_F(ReadFromSocketTest, ReadFromFD_CurlPostRequest) {
    pipe_reguest_to_fd(example_POST_req_ + "Content-Length: 27\r\n\r\n" +
                                            "field1=value1&field2=value2\r\n\r\n");
    v_str req = ReadFromSocket(fd_, 1024);
    EXPECT_EQ(req.size(), 4);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
    EXPECT_EQ(req[1],"Host: foo.example");
    EXPECT_EQ(req[2],"Content-Type: application/x-www-form-urlencoded");
    EXPECT_EQ(req[3],"Content-Length: 27");
}

TEST_F(ReadFromSocketTest, ReadFromFD_Empty) {
    std::string request;
    pipe_reguest_to_fd(request);
    EXPECT_THROW(ReadFromSocket(fd_, 1024), ReadFromSocketFailedException);
}

TEST_F(ReadFromSocketTest, ReadFromFD_NoHeadersHUGE_BODYPostRequest) {
    const std::string &file_str = Utils::FileToString("test_resources/test_body.txt");
    pipe_reguest_to_fd(no_headers_POST_req_ + file_str);
    v_str req = ReadFromSocket(fd_, 1024);
    EXPECT_EQ(req.size(), 1);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
}

TEST_F(ReadFromSocketTest, ReadFromFD_CurlHUGE_BODYPostRequest) {
    const std::string &file_str = Utils::FileToString("test_resources/test_body.txt");
    pipe_reguest_to_fd(example_POST_req_ + file_str);
    v_str req = ReadFromSocket(fd_, 1024);
    EXPECT_EQ(req.size(), 3);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
    EXPECT_EQ(req[1],"Host: foo.example");
    EXPECT_EQ(req[2],"Content-Type: application/x-www-form-urlencoded");
}

TEST_F(ReadFromSocketTest, ThrowBodyTooLarge) {
    const std::string &file_str = Utils::FileToString("test_resources/test_body.txt");
    pipe_reguest_to_fd(example_POST_req_ + file_str);
    v_str req = ReadFromSocket(fd_, 1024);
    EXPECT_EQ(req.size(), 3);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
    EXPECT_EQ(req[1],"Host: foo.example");
    EXPECT_EQ(req[2],"Content-Type: application/x-www-form-urlencoded");
}

class InitTest : public SocketLevelTest {};

TEST_F(InitTest, StandardGetFromFirefox) {
    pipe_reguest_to_fd(firefox_GET_req_);
    Init(fd_);
    EXPECT_EQ(GetMethod(), GET);
    EXPECT_EQ(GetAddress(), "/");
    EXPECT_EQ(GetLastStepUri(), "/");
    EXPECT_EQ(GetFragment(), "");
    EXPECT_EQ(GetParams().size(), 0);
    EXPECT_EQ(GetHeaders().size(), 12);
    EXPECT_EQ(GetHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(GetHeaders().at("User-Agent"), "Mozilla/5.0 (X11; Ubuntu; Linux"
                                             " x86_64; rv:109.0) Gecko/20100101"
                                             " Firefox/115.0");
    EXPECT_EQ(GetHeaders().at("Accept"), "text/html,application/xhtml+xml,"
                                         "application/xml;q=0.9,image/avif,"
                                         "image/webp,*/*;q=0.8");
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

TEST_F(InitTest, example_POST_req_) {
    pipe_reguest_to_fd(example_POST_req_ + "field1=value1&field2=value2");
    Init(fd_);
    EXPECT_EQ(GetMethod(), POST);
    EXPECT_EQ(GetAddress(), "/test");
    EXPECT_EQ(GetLastStepUri(), "/test");
    EXPECT_EQ(GetFragment(), "");
    EXPECT_EQ(GetParams().size(), 0);
    EXPECT_EQ(GetHeaders().size(), 2);
    EXPECT_EQ(GetHeaders().at("Host"), "foo.example");
    EXPECT_EQ(GetHeaders().at("Content-Type"),"application/x-www-form-urlencoded");
}

TEST_F(InitTest, no_headers_POST_req_) {
    pipe_reguest_to_fd(no_headers_POST_req_ + "field1=value1&field2=value2");
    Init(fd_);
    EXPECT_EQ(GetMethod(), POST);
    EXPECT_EQ(GetAddress(), "/test");
    EXPECT_EQ(GetLastStepUri(), "/test");
    EXPECT_EQ(GetFragment(), "");
    EXPECT_EQ(GetParams().size(), 0);
    EXPECT_EQ(GetHeaders().size(), 0);
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
    std::string request = "GET " + uri + " HTTP/1.1\r\n"
                                         "Host: localhost:8080\r\n"
                                         "\r\n";
    pipe_reguest_to_fd(request);
    Init(fd_);
    EXPECT_EQ(GetMethod(), GET);
    EXPECT_EQ(GetAddress(), "/results");
    EXPECT_EQ(GetLastStepUri(), "/results");
    EXPECT_EQ(GetFragment(), "34");
    EXPECT_EQ(GetHeaders().size(), 1);
    EXPECT_EQ(GetHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(GetParams().size(), 5);
    EXPECT_EQ(GetParams().at("search_query"), "pony");
    EXPECT_EQ(GetParams().at("search_type"), "pics");
    EXPECT_EQ(GetParams().at("safe_search"), "off");
    EXPECT_EQ(GetParams().at("nsfw_allow"), "true");
    EXPECT_EQ(GetParams().at("sure"), "yes");
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
    std::string request = "GET " + uri + " HTTP/1.1\r\n"
                                         "Host: localhost:8080\r\n"
                                         "\r\n";
    pipe_reguest_to_fd(request);
    Init(fd_);
    EXPECT_EQ(GetMethod(), GET);
    EXPECT_EQ(GetAddress(), "/results");
    EXPECT_EQ(GetLastStepUri(), "/results");
    EXPECT_EQ(GetFragment(), "34");
    EXPECT_EQ(GetHeaders().size(), 1);
    EXPECT_EQ(GetHeaders().at("Host"), "localhost:8080");
    EXPECT_EQ(GetParams().size(), 4);
    EXPECT_EQ(GetParams().at("search_query"), "pony");
    EXPECT_EQ(GetParams().at("search_type"), "pics");
    EXPECT_EQ(GetParams().at("safe_search"), "off");
    EXPECT_EQ(GetParams().at("nsfw_allow"), "true");
}

// not here
//TEST_F(ReadFromSocketTest, ReadFromFD_NoHeadersPostRequest) {
//    pipe_reguest_to_fd(no_headers_POST_req_ + "field1=value1&field2=value2");
//    v_str req = ReadFromSocket(fd_, 1024);
//    EXPECT_EQ(req.size(), 1);
//    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
//    EXPECT_THROW(ReadBodyToRequest(fd_), );
//}