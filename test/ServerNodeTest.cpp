/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerNodeTest.cpp                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/08 11:13:01 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class ServerNodeTest : public ::testing::Test, public Config {
public:
    explicit ServerNodeTest() : Config() {};
protected:
    ServerConfiguration conf_;
    Node server_;
    Node no_dir_srv_;

    virtual void SetUp() {
        conf_ = ServerConfiguration();
        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "server_name", "example.com" }));
        server_.directives_.push_back(
                v_strings({ "listen", "8182" }));
        server_.directives_.push_back(
                v_strings({ "root", "/some/where/deep/inside" }));
        server_.directives_.push_back(
                v_strings({ "index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "401" , "err.html" }));
    }
};

TEST_F(ServerNodeTest, AllNoChioldnodesOK) {
    EXPECT_NO_THROW(CheckServerSubnodes(server_, conf_));
}

TEST_F(ServerNodeTest, LocationOK) {
    Node loc;

    loc.main_ = v_strings({"location", "/"});
    loc.directives_.push_back({"listen", "8080"});
    loc.directives_.push_back({"server_name", "example.com"});
    loc.directives_.push_back({"client_max_body_size", "2048"});
    loc.directives_.push_back({"error_page", "404", "/404.html"});
    loc.directives_.push_back({"error_page", "500", "502",
                                          "503", "504", "/50x.html"});
    server_.child_nodes_.push_back(loc);
    EXPECT_NO_THROW(CheckServerSubnodes(server_, conf_));
}

TEST_F(ServerNodeTest, LocationKO) {
    Node loc;

    loc.main_ = v_strings({"location", "/", "zz"});

    server_.child_nodes_.push_back(loc);
    EXPECT_THROW(CheckServerSubnodes(server_, conf_), ConfigFileSyntaxError);
}

TEST_F(ServerNodeTest, MultipleLocationSameSeverKO) {
    Location loc = Location("/dup");
    loc.root_ = "resources/locdefault";
    loc.index_.insert("/htmls/index.html");
    conf_.root_loc_.sublocations_.insert(loc);

    Node loc_dup;
    loc_dup.main_ = v_strings({"location", "/dup"});
    loc_dup.directives_.push_back({"root", "resources/locdefault"});
    server_.child_nodes_.push_back(loc_dup);

    EXPECT_THROW(CheckServerSubnodes(server_, conf_),
                 ConfigFileSyntaxError);
}

TEST_F(ServerNodeTest, LimitExceptInSeverNodeKO) {
    Node lim_ex_;
    lim_ex_.main_ = v_strings({"limit_except", "GET" });
    lim_ex_.directives_.push_back({"return", "403"});

    server_.child_nodes_.push_back(lim_ex_);
    EXPECT_THROW(CheckServerSubnodes(server_, conf_), ConfigFileSyntaxError);

}

TEST_F(ServerNodeTest, LocationThrowsAnException) {
    Node lim_ex_;
    lim_ex_.main_ = v_strings({"limit_except", "GET" });
    lim_ex_.directives_.push_back({"return", "403"});

    server_.child_nodes_.push_back(lim_ex_);
    EXPECT_THROW(CheckServerSubnodes(server_, conf_), ConfigFileSyntaxError);

}


//TEST_F(ServerNodeTest, ComponentsTestAllPresent) {
//    server_.main_ = v_strings ({"server"});
//    server_.directives_.push_back(
//            v_strings({ "server_name", "localhost" }));
//    server_.directives_.push_back(
//            v_strings({ "listen", "8080" }));
//    server_.directives_.push_back(
//            v_strings({ "root", "/some/where/deep/inside" }));
//    server_.directives_.push_back(
//            v_strings({ "index", "index.html", "index.htm" }));
//    server_.directives_.push_back(
//            v_strings({ "error_page", "401" , "err.html" }));
//    root_.child_nodes_.push_back(server_);
//    EXPECT_NO_THROW(CheckComponents(root_));
//}
