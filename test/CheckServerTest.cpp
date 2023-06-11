/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    CheckServerTest.cpp                                :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/09 00:40:42 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class CheckServerTest : public ::testing::Test, public Config {
public:
    CheckServerTest() : Config() {};
protected:
    Node server_;
    Node server2_;
    std::vector<ServerConfiguration> servers;

    virtual void SetUp() {
        servers = std::vector<ServerConfiguration>();

        server_ = Node();
        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "server_name", "localhost" }));
        server_.directives_.push_back(
                v_strings({ "root", "/some/where/deep/inside" }));
        server_.directives_.push_back(
                v_strings({ "index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "401" , "err.html" }));server_ = Node();

        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "server_name", "localhost" }));
        server_.directives_.push_back(
                v_strings({ "root", "/some/where/deep/inside" }));
        server_.directives_.push_back(
                v_strings({ "index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "401" , "err.html" }));
    }
};

TEST_F(CheckServerTest, CheckServerTestHasNoPortKO) {
    EXPECT_THROW(CheckServer(servers, server_), ConfigFileSyntaxError);
}

TEST_F(CheckServerTest, CheckServerTestHasPortOK) {
    server_.directives_.push_back({"listen", "8080"});
    EXPECT_NO_THROW(CheckServer(servers, server_));
}

TEST_F(CheckServerTest, CheckServerTestSamePortsKO) {
    server_.directives_.push_back({"listen", "8080"});
    server2_.directives_.push_back({"listen", "8080"});
    EXPECT_NO_THROW(CheckServer(servers, server_));
    EXPECT_THROW(CheckServer(servers, server2_), ConfigFileSyntaxError);
}
