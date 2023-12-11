#include <gtest/gtest.h>
#include "../../../../src/Server/request/ClientRequest.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class ReadFromFDTest : public ::testing::Test, public ClientRequest {
public:
    ReadFromFDTest() : ClientRequest() {}

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

TEST_F(ReadFromFDTest, FFGetRequest) {
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
                          "\n\r";

    pipe_reguest_to_fd(request);
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

TEST_F(ReadFromFDTest, CurlPostRequest) {
    std::string request = "POST /test HTTP/1.1\n\r"
                          "Host: foo.example\n\r"
                          "Content-Type: application/x-www-form-urlencoded\n\r"
                          "Content-Length: 27\n\r"
                          "\n\r"
                          "field1=value1&field2=value2\n\r";

    pipe_reguest_to_fd(request);
    v_str req = ReadFromSocket(fd_);
    EXPECT_EQ(req.size(), 6);
    EXPECT_EQ(req[0],"POST /test HTTP/1.1");
    EXPECT_EQ(req[1],"Host: foo.example");
    EXPECT_EQ(req[2],"Content-Type: application/x-www-form-urlencoded");
    EXPECT_EQ(req[3],"Content-Length: 27");
    EXPECT_EQ(req[4],"");
    EXPECT_EQ(req[5],"field1=value1&field2=value2");
}

TEST_F(ReadFromFDTest, Empty) {
    std::string request = "";

    pipe_reguest_to_fd(request);
    EXPECT_THROW(ReadFromSocket(fd_), ReadFromSocketFailedException);
}
