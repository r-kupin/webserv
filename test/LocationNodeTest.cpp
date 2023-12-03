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
#include <algorithm>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class LocationNodeTest  : public ::testing::Test, public Config {
public:
    explicit LocationNodeTest() : Config() {};
protected:
    ServerConfiguration conf_;
    Node location_root_;

    virtual void SetUp() {
        location_root_ = Node();
        conf_ = ServerConfiguration();

        location_root_.main_ = v_str({"location", "/"});
        location_root_.directives_.push_back({"listen", "8080"});
        location_root_.directives_.push_back({"server_name", "example.com"});
        location_root_.directives_.push_back({"client_max_body_size", "2048"});
        location_root_.directives_.push_back({"error_page", "404", "/404.html"});
        location_root_.directives_.push_back({"error_page", "500", "502",
                                              "503", "504", "/50x.html"});
    }
};

TEST_F(LocationNodeTest, IsLocationTest) {
    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    EXPECT_TRUE(IsLocation(home));
    EXPECT_FALSE(IsLocation(limit_except_get));
}

TEST_F(LocationNodeTest, IsLimitTest) {
    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    EXPECT_FALSE(IsLimitExcept(home));
    EXPECT_TRUE(IsLimitExcept(limit_except_get));
}

TEST_F(LocationNodeTest, IsCorrectLocationTestMoreThan2InMain) {
    Node ff;
    ff.main_ = v_str({"location", "/", "ff" });
    ff.directives_.push_back({"index", "ff.html"});

    EXPECT_TRUE(IsLocation(ff));
    EXPECT_THROW(IsCorrectLocation(ff), ConfigFileSyntaxError);
}

TEST_F(LocationNodeTest, IsCorrectLocationTestNoDirectives) {

    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });

    EXPECT_TRUE(IsLocation(home));
    EXPECT_THROW(IsCorrectLocation(home), ConfigFileSyntaxError);
}

TEST_F(LocationNodeTest, IsCorrectLocationTestNotALocation) {

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    EXPECT_NO_THROW(IsCorrectLocation(limit_except_get));
    EXPECT_FALSE(IsCorrectLocation(limit_except_get));
}

TEST_F(LocationNodeTest, IsCorrectLocationTestGoodLocation) {

    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    EXPECT_NO_THROW(IsCorrectLocation(home));
    EXPECT_TRUE(IsCorrectLocation(home));
}

TEST_F(LocationNodeTest, RootsParentTest) {
    EXPECT_NO_THROW(HandleLocationContext(location_root_, conf_,
                                          conf_.GetRootIt()));
    EXPECT_EQ(*conf_.GetRootIt(), conf_.GetRoot());
    EXPECT_NO_THROW(
            HandleLocationContext(location_root_, conf_, conf_.GetRootIt()));
}

TEST_F(LocationNodeTest, RootSubnodeParentTest) {
    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    home.child_nodes_.push_back(limit_except_get);

    EXPECT_NO_THROW(
            HandleLocationContext(location_root_, conf_, conf_.GetRootIt()));
    EXPECT_NO_THROW(
            HandleLocationContext(home, conf_, conf_.GetRootIt()));
    EXPECT_EQ(conf_.locations_.size(), 1);
    EXPECT_EQ(conf_.locations_.begin(), conf_.GetRootIt());
    EXPECT_EQ(conf_.GetRoot().sublocations_.size(), 1);
    EXPECT_EQ(conf_.GetRoot().sublocations_.begin()->parent_, conf_.GetRootIt());
}

TEST_F(LocationNodeTest, WithLimitExcept) {
    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    home.child_nodes_.push_back(limit_except_get);

    EXPECT_NO_THROW(HandleLocationContext(home, conf_, conf_.GetRootIt()));

    Location HomeLoc = conf_.GetRoot().sublocations_.back();

    EXPECT_EQ(HomeLoc.address_, "/loc_defined_index_not_exist");
    //    TODO test for append, but maybe need replace?
    EXPECT_NE(std::find(HomeLoc.index_.begin(), HomeLoc.index_.end(), "home.html"),
              HomeLoc.index_.end());
    EXPECT_EQ(HomeLoc.limit_except_.except_, std::set<Methods>({GET}));
}

TEST_F(LocationNodeTest, HomeInReDefinedRoot) {
    Node home;

    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    location_root_.child_nodes_.push_back(home);

    EXPECT_NO_THROW(HandleLocationContext(location_root_, conf_,
                                          conf_.GetRootIt()));

    EXPECT_EQ(conf_.GetRoot().address_, "/");
    EXPECT_EQ(conf_.GetRoot().root_ , "resources/root_loc_default");
    EXPECT_NE(std::find(conf_.GetRoot().index_.begin(), conf_.GetRoot().index_.end(),"/htmls/index.html"),
              conf_.GetRoot().index_.end());

    const std::set<ErrPage>::iterator &NotFoundErrPage =
            conf_.GetRoot().error_pages_.find(ErrPage("/404.html", 404));
    EXPECT_NE(NotFoundErrPage, conf_.GetRoot().error_pages_.end());
    EXPECT_EQ(NotFoundErrPage->address_, "/404.html");

    const std::set<ErrPage>::iterator &InternalServerError =
            conf_.GetRoot().error_pages_.find(ErrPage("/50x.html", 500));
    EXPECT_NE(InternalServerError, conf_.GetRoot().error_pages_.end());
    EXPECT_EQ(InternalServerError->address_, "/50x.html");

    EXPECT_EQ(conf_.GetRoot().return_code_ , 0);
    EXPECT_EQ(conf_.GetRoot().return_address_ , "");

    EXPECT_EQ(conf_.GetRoot().sublocations_.begin()->address_, "/loc_defined_index_not_exist");
//    TODO test for append, but maybe need replace?
    EXPECT_NE(std::find(conf_.GetRoot().sublocations_.begin()->index_.begin(),
                        conf_.GetRoot().sublocations_.begin()->index_.end(), "home.html"),
              conf_.GetRoot().sublocations_.begin()->index_.end());
}

TEST_F(LocationNodeTest, CorrectLocationHasWrongOneInside) {
    Node home;
    home.main_ = v_str({"location", "/", "ff" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    home.child_nodes_.push_back(limit_except_get);
    location_root_.child_nodes_.push_back(home);

    EXPECT_THROW(HandleLocationContext(location_root_, conf_, conf_.GetRootIt()),
                 ConfigFileSyntaxError);
}

TEST_F(LocationNodeTest, WrongLimitExcept) {
    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except" });
    limit_except_get.directives_.push_back({"return", "403"});

    home.child_nodes_.push_back(limit_except_get);

    EXPECT_THROW(HandleLocationContext(home, conf_, conf_.GetRootIt()),
                 ConfigFileSyntaxError);
}

TEST_F(LocationNodeTest, MultipleLimitExcept) {
    Node home;
    home.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home.directives_.push_back({"index", "home.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    Node limit_except_post;
    limit_except_post.main_ = v_str({"limit_except", "POST" });
    limit_except_post.directives_.push_back({"deny", "all"});

    home.child_nodes_.push_back(limit_except_get);
    home.child_nodes_.push_back(limit_except_post);

    EXPECT_THROW(HandleLocationContext(home, conf_, conf_.GetRootIt()),
                 ConfigFileSyntaxError);
}

TEST_F(LocationNodeTest, MultipleAddressesUnderTheSameParent) {
    Node home1;
    home1.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home1.directives_.push_back({"index", "home.html"});

    Node home2;
    home2.main_ = v_str({"location", "/loc_defined_index_not_exist" });
    home2.directives_.push_back({"return", "301", "http://localhost:4280/somewhere"});

    location_root_.child_nodes_.push_back(home2);
    location_root_.child_nodes_.push_back(home1);

    EXPECT_THROW(HandleLocationContext(location_root_, conf_, conf_.GetRootIt()),
                 ConfigFileSyntaxError);
}

//class ServerPreconfigTest  : public ::testing::Test, public Config {
//public:
//    explicit ServerPreconfigTest() : Config() {};
//protected:
//    Node root_;
//    Node empty_server_;
//    Node location_home_;
//    Node limit_except_home_;
//    Node location_about_us_;
//    Node location_root_;
//
//    virtual void SetUp() {
//        root_ = Node();
//        empty_server_ = Node();
//        location_home_ = Node();
//        location_about_us_ = Node();
//        limit_except_home_ = Node();
//
//        SetRootDirectives();
//        SetServerDirectives();
//        AddHomeLocationToServer();
//        AddAboutUsLocationToSeServer();
//        UpdateRootLocation();
//
//        root_.child_nodes_.push_back(empty_server_);
//    }
//
//    void UpdateRootLocation() {
//        location_root_.main_ = v_str({"location", "/"});
//        location_root_.directives_.push_back(v_str(
//                                        {"root","/var/www/example.com/html"}));
//        location_root_.directives_.push_back(v_str(
//                                        {"index", "index.html", "index.htm"}));
//        location_root_.directives_.push_back(v_str(
//                    {"error_page", "500", "502", "503", "504", "loc/50x.html"}));
//        empty_server_.child_nodes_.push_back(location_root_);
//    }
//
//    void AddAboutUsLocationToSeServer() {
//        location_about_us_.main_ = v_str({"location", "/loc_defined_index_which_exist"});
//        location_about_us_.directives_.push_back(v_str(
//                {"return", "301", "/about"}));
//
//        empty_server_.child_nodes_.push_back(location_about_us_);
//    }
//
//    void AddHomeLocationToServer() {
//        location_home_.main_ = v_str({"location", "/loc_defined_index_not_exist" });
//        location_home_.directives_.push_back(v_str({"index", "index.html"}));
//
//        limit_except_home_.main_ = v_str({ "limit_except", "GET" });
//        limit_except_home_.directives_.push_back(v_str({"deny", "all"}));
//
//        location_home_.child_nodes_.push_back(limit_except_home_);
//        empty_server_.child_nodes_.push_back(location_home_);
//    }
//
//    void SetRootDirectives() {
//        root_.main_ = v_str ({"main"});
//        root_.directives_.push_back({"include", "mime.types"});
//        root_.directives_.push_back({"default_type", "application/octet-stream"});
//    }
//
//    void SetServerDirectives() {
//        empty_server_.main_ = v_str ({"server"});
//        empty_server_.directives_.push_back(
//                v_str({ "listen", "8080" }));
//        empty_server_.directives_.push_back(
//                v_str({ "server_name", "localhost" }));
//        empty_server_.directives_.push_back(
//                v_str({ "client_max_body_size", "1024" }));
//        empty_server_.directives_.push_back(
//                v_str({ "error_page", "404", "/404.html" }));
//        empty_server_.directives_.push_back(
//                v_str({ "error_page", "500",
//                            "502", "503", "504", "/50x.html" }));
//    }
//};
//
//TEST_F(ServerPreconfigTest, ServerPreconfigTestBasic) {
//
//    std::vector<ServerConfiguration> test_srvrs = CheckComponents(root_);
//    std::vector<ServerConfiguration> exp_srvrs;
//    ServerConfiguration expected_srvr;
//
////  srv
//    expected_srvr.port_ = 8080;
//    expected_srvr.port_str_ = "8080";
//    expected_srvr.server_names_ = v_str({"localhost"});
//    expected_srvr.server_name_ = std::string("localhost");
//    expected_srvr.client_max_body_size_ = 1024;
//
////    RootLocation
//    expected_srvr.locations_.address_ = "/";
//    expected_srvr.locations_.root_ = "/var/www/example.com/html";
//    expected_srvr.locations_.index_.clear();
//    expected_srvr.locations_.index_.push_back("index.html");
//    expected_srvr.locations_.index_.push_back("index.htm");
//    expected_srvr.locations_.error_pages_.clear();
//    expected_srvr.locations_.error_pages_.insert(
//            ErrPage("/htmls/403.html", 403));
//    expected_srvr.locations_.error_pages_.insert(
//            ErrPage("/404.html", 404));
//    expected_srvr.locations_.error_pages_.insert(
//            ErrPage("loc/50x.html", 500));
//    expected_srvr.locations_.error_pages_.insert(
//            ErrPage("loc/50x.html", 502));
//    expected_srvr.locations_.error_pages_.insert(
//            ErrPage("loc/50x.html", 503));
//    expected_srvr.locations_.error_pages_.insert(
//            ErrPage("loc/50x.html", 504));
//
//    Location ab_us = Location("/loc_defined_index_which_exist");
//    ab_us.return_code_ = 301;
//    ab_us.return_address_ = "/about";
//    expected_srvr.locations_.sublocations_.push_back(ab_us);
//
//    Location home = Location("/loc_defined_index_not_exist");
//    home.index_.push_back("index.html");
//    home.limit_except_.return_code_ = 403;
//    home.limit_except_.except_.insert(GET);
//    expected_srvr.locations_.sublocations_.push_back(home);
//
//    exp_srvrs.push_back(expected_srvr);
//    EXPECT_EQ(test_srvrs, exp_srvrs);
//}
//

//TEST_F(ServerPreconfigTest, UpdateIndexTest) {
//    std::vector<ServerConfiguration> test_srvrs = CheckComponents(root_);
//    ServerConfiguration & srv = test_srvrs[0];
//    v_str directive({"index", "test.htm"});
//
//    srv.locations_.index_ = std::set<std::string>({"index.html"});
//    srv.locations_.default_index_ = true;
//
//    UpdateIndex(directive, srv.locations_);
//
//    EXPECT_NE(srv.locations_.index_.find("test.htm"),
//              srv.locations_.index_.end());
//    EXPECT_FALSE(srv.locations_.default_index_);
//}
//
//TEST_F(ServerPreconfigTest, HandleLocationFailedLocationTest) {
//    Node srv_node, loc_node, root;
//
//    srv_node.main_ = v_str ({"server"});
//    srv_node.directives_.push_back(
//            v_str({ "listen", "8080" }));
//    srv_node.directives_.push_back(
//            v_str({ "server_name", "localhost" }));
//    srv_node.directives_.push_back(
//            v_str({ "client_max_body_size", "1024" }));
//    srv_node.directives_.push_back(
//            v_str({ "error_page", "404", "/404.html" }));
//    srv_node.directives_.push_back(
//            v_str({ "error_page", "500",
//                        "502", "503", "504", "/50x.html" }));
//    loc_node.main_ = v_str({"location", "/"});
//    srv_node.child_nodes_.push_back(loc_node);
//    root.child_nodes_.push_back(srv_node);
//
//    EXPECT_THROW(CheckComponents(root), ConfigFileSyntaxError);
//