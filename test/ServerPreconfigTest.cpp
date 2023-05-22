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
    Node location_home_;
    Node limit_except_home_;
    Node location_about_us_;
    Node location_root_;

    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
        location_home_ = Node();
        location_about_us_ = Node();
        limit_except_home_ = Node();

        SetRootDirectives();
        SetServerDirectives();
        AddHomeLocationToServer();
        AddAboutUsLocationToSeServer();
        UpdateRootLocation();

        root_.child_nodes_.push_back(server_);
    }

    void UpdateRootLocation() {
        location_root_.main_ = v_strings({"location", "/"});
        location_root_.directives_.push_back(v_strings(
                                        {"root","/var/www/example.com/html"}));
        location_root_.directives_.push_back(v_strings(
                                        {"index", "index.html", "index.htm"}));
        location_root_.directives_.push_back(v_strings(
                    {"error_page", "500", "502", "503", "504", "loc/50x.html"}));
        server_.child_nodes_.push_back(location_root_);
    }

    void AddAboutUsLocationToSeServer() {
        location_about_us_.main_ = v_strings({"location", "/about-us"});
        location_about_us_.directives_.push_back(v_strings(
                {"return", "301", "/about"}));

        server_.child_nodes_.push_back(location_about_us_);
    }

    void AddHomeLocationToServer() {
        location_home_.main_ = v_strings({"location", "/home" });
        location_home_.directives_.push_back(v_strings({"index", "index.html"}));

        limit_except_home_.main_ = v_strings({ "limit_except", "GET" });
        limit_except_home_.directives_.push_back(v_strings({"deny", "all"}));

        location_home_.child_nodes_.push_back(limit_except_home_);
        server_.child_nodes_.push_back(location_home_);
    }

    void SetRootDirectives() {
        root_.main_ = v_strings ({"main"});
        root_.directives_.push_back({"include", "mime.types"});
        root_.directives_.push_back({"default_type", "application/octet-stream"});
    }

    void SetServerDirectives() {
        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "listen", "8080" }));
        server_.directives_.push_back(
                v_strings({ "server_name", "localhost" }));
        server_.directives_.push_back(
                v_strings({ "client_max_body_size", "1024" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "404", "/404.html" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "500",
                            "502", "503", "504", "/50x.html" }));
    }
};

TEST_F(ServerPreconfigTest, ServerPreconfigTestBasic) {

    std::vector<ServerConfiguration> test_srvrs = CheckComponents(root_);
    std::vector<ServerConfiguration> exp_srvrs;
    ServerConfiguration expected_srvr;

//  srv
    expected_srvr.port_ = 8080;
    expected_srvr.port_str_ = "8080";
    expected_srvr.server_names_ = v_strings({"localhost"});
    expected_srvr.hostname_ = std::string("localhost");
    expected_srvr.client_max_body_size_ = 1024;

//    RootLocation
    expected_srvr.root_loc_.address_ = "/";
    expected_srvr.root_loc_.root_ = "/var/www/example.com/html";
    expected_srvr.root_loc_.index_.insert("index.html");
    expected_srvr.root_loc_.index_.insert("index.htm");
    expected_srvr.root_loc_.error_pages_.clear();
    expected_srvr.root_loc_.error_pages_.insert(
            ErrPage("/htmls/403.html", 403));
    expected_srvr.root_loc_.error_pages_.insert(
            ErrPage("/404.html", 404));
    expected_srvr.root_loc_.error_pages_.insert(
            ErrPage("loc/50x.html", 500));
    expected_srvr.root_loc_.error_pages_.insert(
            ErrPage("loc/50x.html", 502));
    expected_srvr.root_loc_.error_pages_.insert(
            ErrPage("loc/50x.html", 503));
    expected_srvr.root_loc_.error_pages_.insert(
            ErrPage("loc/50x.html", 504));

    Location ab_us = Location("/about-us");
    ab_us.return_code_ = 301;
    ab_us.return_address_ = "/about";
    expected_srvr.root_loc_.sublocations_.insert(ab_us);

    Location home = Location("/home");
    home.index_.insert("index.html");
    home.limit_except_.return_code_ = 403;
    home.limit_except_.except_.insert(GET);
    expected_srvr.root_loc_.sublocations_.insert(home);

    exp_srvrs.push_back(expected_srvr);
    EXPECT_EQ(test_srvrs, exp_srvrs);
}