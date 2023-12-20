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
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Config/config/ConfigExceptions.h"

class ConfigServerSubcontextsHandlingTest : public ::testing::Test, public Config {
public:
    explicit ConfigServerSubcontextsHandlingTest() : Config() {};
protected:
    ServerConfiguration conf_;
    Node server_;
    Node no_dir_srv_;

    virtual void SetUp() {
        conf_ = ServerConfiguration();
        server_.main_ = v_str ({"server"});
        server_.directives_.push_back(
                v_str({"server_name", "example.com" }));
        server_.directives_.push_back(
                v_str({"listen", "8182" }));
        server_.directives_.push_back(
                v_str({"root", "/not/depends/on/config" }));
        server_.directives_.push_back(
                v_str({"index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_str({"error_page", "401" , "err.html" }));
    }
};

TEST_F(ConfigServerSubcontextsHandlingTest, LocationOK) {
    Node loc;

    loc.main_ = v_str({"location", "/"});
    loc.directives_.push_back({"listen", "8080"});
    loc.directives_.push_back({"server_name", "example.com"});
    loc.directives_.push_back({"client_max_body_size", "2048"});
    loc.directives_.push_back({"error_page", "404", "/404.html"});
    loc.directives_.push_back({"error_page", "500", "502",
                                          "503", "504", "/50x.html"});
    server_.child_nodes_.push_back(loc);
    EXPECT_NO_THROW(CheckServerSubnodes(server_.child_nodes_, conf_));
}

TEST_F(ConfigServerSubcontextsHandlingTest, LocationKO) {
    Node loc;

    loc.main_ = v_str({"location", "/", "zz"});

    server_.child_nodes_.push_back(loc);
    EXPECT_THROW(CheckServerSubnodes(server_.child_nodes_, conf_), ConfigFileSyntaxError);
}

// todo non-root location redefinition
//TEST_F(ConfigServerSubcontextsHandlingTest, MultipleLocationSameSeverKO) {
//    Location loc = Location("/dup");
//    loc.root_ = "/not/depends/on/config";
//    loc.index_.push_back("/htmls/index.html");
//    conf_.GetRoot().sublocations_.push_back(loc);
//
//    Node loc_dup;
//    loc_dup.main_ = v_str({"location", "/dup"});
//    loc_dup.directives_.push_back({"root", "/not/depends/on/config"});
//    server_.child_nodes_.push_back(loc_dup);
//
//    EXPECT_THROW(CheckServerSubnodes(server_.child_nodes_, conf_), ConfigFileSyntaxError);
//}

TEST_F(ConfigServerSubcontextsHandlingTest, LimitExceptInSeverNodeKO) {
    Node lim_ex_;
    lim_ex_.main_ = v_str({"limit_except", "GET" });
    lim_ex_.directives_.push_back({"return", "403"});

    server_.child_nodes_.push_back(lim_ex_);
    EXPECT_THROW(CheckServerSubnodes(server_.child_nodes_, conf_), ConfigFileSyntaxError);
}

TEST_F(ConfigServerSubcontextsHandlingTest, LocationThrowsAnException) {
    Node lim_ex_;
    lim_ex_.main_ = v_str({"limit_except", "GET" });
    lim_ex_.directives_.push_back({"return", "403"});

    server_.child_nodes_.push_back(lim_ex_);
    EXPECT_THROW(CheckServerSubnodes(server_.child_nodes_, conf_), ConfigFileSyntaxError);
}
