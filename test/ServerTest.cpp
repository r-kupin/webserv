#include <gtest/gtest.h>
#include "../src/Server/Server.h"
#include "../src/Server/ServerExceptions.h"


class ClienRequestTest : public ::testing::Test {
protected:
    int fd_;

protected:
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

TEST_F(ClienRequestTest, StandardFromFirefox) {
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
        "Sec-Fetch-User: ?1\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/");
    EXPECT_EQ(cl_req.headers_.size(), 12);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
}

TEST_F(ClienRequestTest, OneParam) {
    std::string uri = "/results?search_query=pony";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_EQ(cl_req.params_.size(), 1);
    EXPECT_EQ(cl_req.params_["search_query"], "pony");
}

TEST_F(ClienRequestTest, MoarParamz) {
    std::string uri = "/results?"
                      "search_query=pony&"
                      "search_type=pics&"
                      "safe_search=off&"
                      "nsfw_allow=true&"
                      "sure=yes";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_EQ(cl_req.params_.size(), 5);
    EXPECT_EQ(cl_req.params_["search_query"], "pony");
    EXPECT_EQ(cl_req.params_["nsfw_allow"], "true");
}

TEST_F(ClienRequestTest, IncompleteQuerry1) {
    std::string uri = "/results?"
                      "search_query=";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_TRUE(cl_req.params_.empty());
}

TEST_F(ClienRequestTest, IncompleteQuerry2) {
    std::string uri = "/results?"
                      "search_query=pony&"
                      "search_type=&"
                      "safe_search=off&"
                      "nsfw_allow=true&"
                      "sure=yes";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_EQ(cl_req.params_.size(), 4);
    EXPECT_EQ(cl_req.params_["search_query"], "pony");
    EXPECT_EQ(cl_req.params_["nsfw_allow"], "true");
    EXPECT_EQ(cl_req.params_.find("search_type"), cl_req.params_.end());
}

TEST_F(ClienRequestTest, OnlyAmpersand) {
    std::string uri = "/results?&";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_TRUE(cl_req.params_.empty());
}

TEST_F(ClienRequestTest, OnlyAssign) {
    std::string uri = "/results?=";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_TRUE(cl_req.params_.empty());
}

TEST_F(ClienRequestTest, AsignAndAmpersand) {
    std::string uri = "/results?=&";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_TRUE(cl_req.params_.empty());
}

TEST_F(ClienRequestTest, AmpersandAndAsign) {
    std::string uri = "/results?&=";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
                                         "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_TRUE(cl_req.params_.empty());
}

TEST_F(ClienRequestTest, QuerryWithQestionMark) {
    std::string uri = "/results?"
                      "search_query=pony&"
                      "search_type=pics&"
                      "safe_search=o?ff&"
                      "nsfw_allow=true&"
                      "sure=yes";
    std::string request = "GET " + uri + " HTTP/1.1\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);
    EXPECT_EQ(cl_req.method_, GET);
    EXPECT_EQ(cl_req.address_, "/results");
    EXPECT_EQ(cl_req.headers_.size(), 1);
    EXPECT_EQ(cl_req.headers_["Host"], "localhost:8080");
    EXPECT_EQ(cl_req.params_.size(), 5);
    EXPECT_EQ(cl_req.params_["search_query"], "pony");
    EXPECT_EQ(cl_req.params_["safe_search"], "o?ff");
}

TEST_F(ClienRequestTest, HTTP2) {
    std::string request ="GET / HTTP/2.0\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    EXPECT_THROW(ClientRequest badd(fd_), HTTPVersionNotSupportedException);
}

TEST_F(ClienRequestTest, PUTMETHOD) {
    std::string request ="PUT / HTTP/2.0\n\r"
        "Host: localhost:8080\n\r";

    pipe_reguest_to_fd(request);
    EXPECT_THROW(ClientRequest badd(fd_), UnsupportedClientMethodException);
}

TEST_F(ClienRequestTest, EmptyRequest) {
    std::string request = "";

    pipe_reguest_to_fd(request);
    EXPECT_ANY_THROW(ClientRequest badd(fd_));
}

TEST_F(ClienRequestTest, SomeGarbage) {
    std::string request = "laskdijas;dlifjasd[;o'lf";

    pipe_reguest_to_fd(request);
    EXPECT_ANY_THROW(ClientRequest badd(fd_));
}

class RequestHandlingTest : public ::testing::Test, public Server {
public:
    explicit RequestHandlingTest() : Server(
            *Config("resources/default/nginx.conf").getServers().begin()) {}
};

TEST_F(RequestHandlingTest, FindRootLocation) {
    int code;
    std::string addr = "/";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_, addr);
    EXPECT_EQ(code, 200);
}

TEST_F(RequestHandlingTest, FindHomeLocation) {
    int code;
    std::string addr = "/home";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_,
              "/home");
    EXPECT_EQ(code, 200);
}

TEST_F(RequestHandlingTest, FindAboutUs_ContactsLocation) {
    int code;
    std::string addr = "/about-us/contacts";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_,
              "/contacts");
    EXPECT_EQ(code, 200);
}

TEST_F(RequestHandlingTest, FindUploadsSomethWhatewerSomething) {
    int code;
    std::string addr = "/uploads/something/whatever";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_,
              "/whatever");
    EXPECT_EQ(code, 200);
}

TEST_F(RequestHandlingTest, BadRequest) {
    int code;
    std::string addr = "kjhsdklhfg";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_, "/");
    EXPECT_EQ(code, 400);
}

TEST_F(RequestHandlingTest, RootNotFound) {
    int code;
    std::string addr = "/kjhsdklhfg";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_, "/");
    EXPECT_EQ(code, 404);
}

TEST_F(RequestHandlingTest, SublocationNotFound) {
    int code;
    std::string addr = "/home/kjhsdklhfg";

    EXPECT_EQ(FindLocation(addr, getConfig().GetRoot(), code).address_,
              "/home");
    EXPECT_EQ(code, 404);
}