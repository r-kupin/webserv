#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"

class InitTest : public ::testing::Test, public ClientRequest {
public:
    InitTest() : ClientRequest() {}

protected:
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
};

TEST_F(InitTest, StandardGetFromFirefox) {
    std::string request = "GET / HTTP/1.1\n\r"
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
                          "";

    pipe_reguest_to_fd(request);
    Init(fd_);
    EXPECT_EQ(method_, GET);
    EXPECT_EQ(addr_, "/");
    EXPECT_EQ(headers_.size(), 12);
    EXPECT_EQ(headers_["Host"], "localhost:8080");
}