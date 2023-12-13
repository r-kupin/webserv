#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../../../../src/Server/ServerExceptions.h"


class LocationSynthesingTest : public ::testing::Test, public Server {
public:
    explicit LocationSynthesingTest()
    : Server(*Config("test_resources/test1/nginx.conf").
                                                    getServers().begin()) {};
protected:
    int fd_;

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
    const Location &found = RecursiveSearch("/loc_defined_index_which_exist",
                                            getConfig().GetRoot(),
                                            status);
    // root = example/htmls/loc_defined_index_which_exist
    // path = test_resources/test1/example/htmls/loc_defined_index_which_exist/loc_defined_index_which_exist
    EXPECT_TRUE(CheckFilesystem(found.root_, "test_resources/test1/"));
    EXPECT_FALSE(CheckFilesystem(found.root_, "test_resources/test2/"));
}

TEST_F(LocationSynthesingTest, CheckFoundLocationAccessLimitation) {
    std::string status;
    const Location &found = RecursiveSearch("/uploads/something/whatever",
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
    const Location &found = RecursiveSearch(loc,
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
    const Location &found = RecursiveSearch(loc,
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
