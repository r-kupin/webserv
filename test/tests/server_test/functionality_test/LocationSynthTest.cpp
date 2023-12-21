/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSynthTest.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:42:40 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

class LocationSynthTest : public ::testing::Test, public Server {
public:
    explicit LocationSynthTest()
            : Server(Config("test_resources/nested_locations/nginx.conf").getConstServers().front()) {};
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
//
//TEST_F(LocationSynthTest, Root) {
//    pipe_reguest_to_fd("GET / HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, Loc_1) {
//    pipe_reguest_to_fd("GET /loc_1 HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_1_loc_XX) {
//    pipe_reguest_to_fd("GET /loc_1/loc_XX HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_1_loc_3) {
//    pipe_reguest_to_fd("GET /loc_1/loc_3/ HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_1_loc_2) {
//    pipe_reguest_to_fd("GET /loc_1/loc_2/ HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_1_loc_3_loc_3_in_loc_1_404_html) {
//    pipe_reguest_to_fd("GET /loc_1/loc_3/loc_3_in_loc_1_404.html HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_2) {
//    pipe_reguest_to_fd("GET /loc_2 HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_5) {
//    pipe_reguest_to_fd("GET /loc_5 HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_5_loc_7) {
//    pipe_reguest_to_fd("GET /loc_5/loc_7 HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, loc_0X) {
//    pipe_reguest_to_fd("GET /loc_0X HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
//
//TEST_F(LocationSynthTest, XXX) {
//    pipe_reguest_to_fd("GET /XXX HTTP/1.1");
//    ClientRequest request(fd_);
//
//    Location synth = SynthesizeHandlingLocation(request);
//}
