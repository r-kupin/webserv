/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    HandleRequestTest.cpp                              :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/07 17:13:21 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../ConfigFiles.h"
#include "../../../../src/Server/request/RequestExceptions.h"

class SimpleIndexCheckTest : public ::testing::Test, public Server {
public:
    explicit SimpleIndexCheckTest(): Server(simple.getConstServers().front()) {};
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

TEST_F(SimpleIndexCheckTest, GetRoot) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET / HTTP/1.1\r\n");
    request.Init(fd_, GetConfig().GetClientMaxBodySize());
    response_location = SynthesizeHandlingLocation(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 200 OK");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetBodyStr(), "<!DOCTYPE html>\n"
                                     "<html lang=\"en\">\n"
                                     "<head>\n"
                                     "    <meta charset=\"UTF-8\">\n"
                                     "    <title>root_index.html</title>\n"
                                     "</head>\n"
                                     "<body>\n"
                                     "This is root_index.html\n"
                                     "</body>\n"
                                     "</html>");
}

TEST_F(SimpleIndexCheckTest, PostRootBodyWithinLimits) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("POST / HTTP/1.1\r\n\r\nthis is body\n\r");
    request.Init(fd_, GetConfig().GetClientMaxBodySize());
    response_location = SynthesizeHandlingLocation(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 200 OK");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetBodyStr(), "<!DOCTYPE html>\n"
                                     "<html lang=\"en\">\n"
                                     "<head>\n"
                                     "    <meta charset=\"UTF-8\">\n"
                                     "    <title>root_index.html</title>\n"
                                     "</head>\n"
                                     "<body>\n"
                                     "This is root_index.html\n"
                                     "</body>\n"
                                     "</html>");
}

TEST_F(SimpleIndexCheckTest, GetProvokeRedirect) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET /loc_0 HTTP/1.1\r\n");
    request.Init(fd_, GetConfig().GetClientMaxBodySize());
    response_location = SynthesizeHandlingLocation(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 301 Moved Permanently");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetHeaders().find("Location")->second, "http://localhost:4280/loc_0/");
}