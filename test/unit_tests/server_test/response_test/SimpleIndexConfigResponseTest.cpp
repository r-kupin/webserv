/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SimpleIndexConfigResponseTest.cpp                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/08 12:42:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../ConfigFiles.h"

class SimpleIndexConfigResponseTest : public ::testing::Test, public Server {
public:
    explicit SimpleIndexConfigResponseTest()
            : Server(simple_index_check.getConstServers().front()) {}
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

TEST_F(SimpleIndexConfigResponseTest, LocX) {
Location        response_location;
ClientRequest   request;
ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

pipe_reguest_to_fd("GET /loc_X HTTP/1.1\r\n");
    request.Init(fd_, NULL);
response_location = ProcessRequest(request);
response.ComposeResponse(response_location);

EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 404 Not Found");
EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
}

TEST_F(SimpleIndexConfigResponseTest, LocXIndex) {
Location        response_location;
ClientRequest   request;
ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

pipe_reguest_to_fd("GET /loc_X/ HTTP/1.1\r\n");
    request.Init(fd_, NULL);
response_location = ProcessRequest(request);
response.ComposeResponse(response_location);

EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 404 Not Found");
EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
}

TEST_F(SimpleIndexConfigResponseTest, Loc4) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET /loc_4 HTTP/1.1\r\n");
    request.Init(fd_, NULL);
    response_location = ProcessRequest(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 301 Moved Permanently");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
    EXPECT_EQ(response.GetHeaders().find("Location")->second,
              "http://localhost:" + Utils::NbrToString(GetConfig().GetPort()) + "/loc_4/");
}

TEST_F(SimpleIndexConfigResponseTest, Loc4Index) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET /loc_4/ HTTP/1.1\r\n");
    request.Init(fd_, NULL);
    response_location = ProcessRequest(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 403 Forbidden");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
}

TEST_F(SimpleIndexConfigResponseTest, Loc4File) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET /loc_4/index.html HTTP/1.1\r\n");
    request.Init(fd_, NULL);
    response_location = ProcessRequest(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 200 OK");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
}

TEST_F(SimpleIndexConfigResponseTest, loc1) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET /loc_1 HTTP/1.1\r\n");
    request.Init(fd_, NULL);
    response_location = ProcessRequest(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 301 Moved Permanently");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
    EXPECT_EQ(response.GetHeaders().find("Location")->second,
              "http://localhost:" + Utils::NbrToString(GetConfig().GetPort())
              + "/loc_1/");
}

TEST_F(SimpleIndexConfigResponseTest, loc1Index) {
    Location        response_location;
    ClientRequest   request;
    ServerResponse  response(GetConfig().GetServerName(), GetConfig().GetPort());

    pipe_reguest_to_fd("GET /loc_1/ HTTP/1.1\r\n");
    request.Init(fd_, NULL);
    response_location = ProcessRequest(request);
    response.ComposeResponse(response_location);

    EXPECT_EQ(response.GetTopHeader(), "HTTP/1.1 200 OK");
    EXPECT_EQ(response.GetHeaders().find("Server")->second, "WebServ");
    EXPECT_EQ(response.GetHeaders().find("Content-Type")->second, "text/html");
    EXPECT_EQ(response.GetBodyStr(), "<!DOCTYPE html>\n"
                                     "<html lang=\"en\">\n"
                                     "<head>\n"
                                     "    <meta charset=\"UTF-8\">\n"
                                     "    <title>index</title>\n"
                                     "</head>\n"
                                     "<body>\n"
                                     "index in loc 4\n"
                                     "</body>\n"
                                     "</html>");
}