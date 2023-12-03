#include <gtest/gtest.h>
#include "../src/Server/Server.h"
#include "../src/Server/ServerExceptions.h"

class ClienRequestTest : public ::testing::Test {
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
            *Config("test_resources/test1/default/nginx.conf").getServers().begin()) {}
};

TEST_F(RequestHandlingTest, FindRootLocation) {
    std::string status;
    std::string addr = "/";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              addr);
    EXPECT_EQ(status, "found");
}

TEST_F(RequestHandlingTest, FindHomeLocation) {
    std::string status;
    std::string addr = "/loc_defined_index_not_exist";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              "/loc_defined_index_not_exist");
    EXPECT_EQ(status, "found");
}

TEST_F(RequestHandlingTest,
       Find_loc_defined_index_which_exist_ContactsLocation) {
    std::string status;
    std::string addr = "/loc_defined_index_which_exist/contacts";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              "/contacts");
    EXPECT_EQ(status, "found");
}

TEST_F(RequestHandlingTest, FindUploadsSomethWhatewerSomething) {
    std::string status;
    std::string addr = "/uploads/something/whatever";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              "/whatever");
    EXPECT_EQ(status, "found");
}

TEST_F(RequestHandlingTest, BadRequest) {
    std::string status;
    std::string addr = "kjhsdklhfg";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              "/");
    EXPECT_EQ(status, "uri misconfigured");
}

TEST_F(RequestHandlingTest, RootNotFound) {
    std::string status;
    std::string addr = "/kjhsdklhfg";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              "/");
    EXPECT_EQ(status, "not found");
}

TEST_F(RequestHandlingTest, SublocationNotFound) {
    std::string status;
    std::string addr = "/loc_defined_index_not_exist/kjhsdklhfg";

    EXPECT_EQ(FindSublocation(addr, getConfig().GetRoot(), status).address_,
              "/loc_defined_index_not_exist");
    EXPECT_EQ(status, "not found");
}

class LocationSynthesingTest : public ::testing::Test, public Server {
public:
    explicit LocationSynthesingTest() : Server(
            *Config("test_resources/test1/default/nginx.conf").getServers()
            .begin())
            {};
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

TEST_F(LocationSynthesingTest, CheckFoundLocationPathDoesntExist) {
    std::string status;
    const Location &found = FindSublocation("/loc_defined_index_which_exist",
                                            getConfig().GetRoot(),
                                            status);
    // root = example/htmls/loc_defined_index_which_exist
    // path = test_resources/test1/example/htmls/loc_defined_index_which_exist/loc_defined_index_which_exist
    EXPECT_TRUE(CheckFilesystem(found.root_, "test_resources/test1/"));
    EXPECT_FALSE(CheckFilesystem(found.root_, "test_resources/test2/"));
}

TEST_F(LocationSynthesingTest, CheckFoundLocationAccessLimitation) {
    std::string status;
    const Location &found = FindSublocation("/uploads/something/whatever",
                                            getConfig().GetRoot(),
                                            status);
    EXPECT_TRUE(CheckLimitedAccess(found, Methods::GET));
    EXPECT_FALSE(CheckLimitedAccess(found, Methods::POST));

    EXPECT_TRUE(CheckLimitedAccess(*found.parent_, Methods::GET));
    EXPECT_FALSE(CheckLimitedAccess(*found.parent_, Methods::POST));
}

TEST_F(LocationSynthesingTest,
       SynthesiseFor_ExactMatch_DirectoryExist_IndexDefinedButDontExist) {
    std::string loc = "/loc_defined_index_not_exist";
    std::string status;
    const Location &found = FindSublocation(loc,
                                            getConfig().GetRoot(),
                                            status);

    std::string request = "GET " + loc + " HTTP/1.1\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);

    Location synth(found);
    synth = SynthFoundExact(cl_req, found, synth,
                            "test_resources/test1/");
    EXPECT_EQ(synth.return_code_, 403);
}

TEST_F(LocationSynthesingTest,
       SynthesiseFor_ExactMatch_DirectoryExist_IndexDefined_AndExist) {
    std::string loc = "/loc_defined_index_which_exist";
    std::string status;
    const Location &found = FindSublocation(loc,
                                            getConfig().GetRoot(),
                                            status);

    std::string request = "GET " + loc + " HTTP/1.1\n\r";

    pipe_reguest_to_fd(request);
    ClientRequest cl_req(fd_);

    Location synth(found);
    synth = SynthFoundExact(cl_req, found, synth,
                            "test_resources/test1/");
    EXPECT_EQ(synth.return_code_, 200);
}