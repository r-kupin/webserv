/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerPreconfigTest.cpp                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/23 15:45:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class ServerPreconfigTest  : public ::testing::Test, public Config {
public:
    explicit ServerPreconfigTest() : Config() {};
protected:
    Node root_;
    Node server_;
    Node location_;
    Node limit_except_;

    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
        location_ = Node();
        limit_except_ = Node();

        root_.main_ = v_strings ({"main"});
        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "server_name", "localhost" }));
        server_.directives_.push_back(
                v_strings({ "listen", "8080" }));
//        server_.directives_.push_back(
//                v_strings({ "root", "/some/where" }));
        server_.directives_.push_back(
                v_strings({ "index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "500",
                            "502", "503", "504", "/50x.html" }));

        location_.main_ =
                v_strings({"location", "/" });
        location_.directives_.push_back(
                v_strings({"root", "/some/where"}));

        limit_except_.main_ =
                v_strings({ "limit_except", "GET", "POST", "DELETE" });
        limit_except_.directives_.push_back(
                v_strings({"deny", "all"}));

        location_.child_nodes_.push_back(limit_except_);
        server_.child_nodes_.push_back(location_);
        root_.child_nodes_.push_back(server_);
    }
};

TEST_F(ServerPreconfigTest, ServerPreconfigTestBasic) {

    std::vector<ServerConfiguration> test_srvrs = CheckComponents(root_);
    ServerConfiguration expected_srvr;

//  srv
    expected_srvr.port_ = 8080;
    expected_srvr.index_ = v_strings({"index.html", "index.htm"});
    expected_srvr.server_names_ = v_strings({"localhost"});
    expected_srvr.client_max_body_size_ = 0;

//  err
    ErrPage err;
    err.address_ = "/50x.html";
    err.code_ = std::vector<int>({500, 502, 503, 504});

//  loc
    Location loc;
    loc.limit_except_methods_.insert(DELETE);
    loc.limit_except_methods_.insert(GET);
    loc.limit_except_methods_.insert(POST);
    loc.limit_except_action_ = DENY;
    loc.return_code_ = 0;
    loc.return_address_ = "";
    loc.root_ = "/some/where";

    expected_srvr.locations_.push_back(loc);
    expected_srvr.error_pages_.push_back(err);

//  tests
    std::vector<ServerConfiguration> expected_srvrs;
    expected_srvrs.push_back(expected_srvr);

    EXPECT_EQ(test_srvrs, expected_srvrs);
}