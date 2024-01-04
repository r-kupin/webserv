/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSynthTest_SimpleIndex.cpp                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:42:40 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../ConfigFiles.h"

class LocationSynthTest_SimpleIndex : public ::testing::Test, public Server {
public:
    explicit LocationSynthTest_SimpleIndex()
            : Server(simple_index_check.getConstServers().front()) {};
protected:
    int fd_;

    virtual void TearDown() {
        close(fd_);
    }

    void pipe_reguest_to_fd(const std::string& request) {
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

TEST_F(LocationSynthTest_SimpleIndex, Root) {
    pipe_reguest_to_fd("GET / HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/index_1.html");
}

TEST_F(LocationSynthTest_SimpleIndex, Loc_1) {
    pipe_reguest_to_fd("GET /loc_1 HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 301);
    EXPECT_EQ(synth.return_internal_address_, "/loc_1/");
}

TEST_F(LocationSynthTest_SimpleIndex, loc_1_) {
    pipe_reguest_to_fd("GET /loc_1/ HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/loc_4/index.html");
}

TEST_F(LocationSynthTest_SimpleIndex, loc_2) {
    pipe_reguest_to_fd("GET /loc_2 HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 301);
    EXPECT_EQ(synth.return_internal_address_, "/loc_2/");
}

TEST_F(LocationSynthTest_SimpleIndex, loc_2_) {
    pipe_reguest_to_fd("GET /loc_2/ HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 403);
}

TEST_F(LocationSynthTest_SimpleIndex, loc_3) {
    pipe_reguest_to_fd("GET /loc_3 HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 301);
    EXPECT_EQ(synth.return_internal_address_, "/loc_3/");
}

TEST_F(LocationSynthTest_SimpleIndex, loc_3_) {
    pipe_reguest_to_fd("GET /loc_3/ HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 403);
}

TEST_F(LocationSynthTest_SimpleIndex, loc_4) {
    pipe_reguest_to_fd("GET /loc_4 HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 301);
    EXPECT_EQ(synth.return_internal_address_, "/loc_4/");
}

TEST_F(LocationSynthTest_SimpleIndex, loc_4_) {
    pipe_reguest_to_fd("GET /loc_4/ HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 403);
}

TEST_F(LocationSynthTest_SimpleIndex, loc_4_index) {
    pipe_reguest_to_fd("GET /loc_4/index.html HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/loc_4/index.html");
}

TEST_F(LocationSynthTest_SimpleIndex, loc_X) {
    pipe_reguest_to_fd("GET /loc_X HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 404);
}

TEST_F(LocationSynthTest_SimpleIndex, loc_X_) {
    pipe_reguest_to_fd("GET /loc_X/ HTTP/1.1\n\r\n\r");
    ClientRequest request(fd_);

    Location synth = SynthesizeHandlingLocation(request);
    EXPECT_EQ(synth.return_code_, 404);
}
