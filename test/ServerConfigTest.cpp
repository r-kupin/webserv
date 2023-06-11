/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerConfigTest.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/04 10:52:39 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"
#include "../src/Server/ServerExceptions.h"

class ServerConfigTest : public ::testing::Test, public ServerConfiguration {
public:
    explicit ServerConfigTest() : ServerConfiguration() {};
protected:
    std::vector<v_strings> directives_;

    virtual void SetUp() {
        port_ = false;
        directives_ = std::vector<v_strings>();
    }
};

TEST_F(ServerConfigTest, DetfaultTest) {
    EXPECT_EQ(client_max_body_size_, 1024);
    EXPECT_EQ(default_index_, true);
    EXPECT_EQ(default_hostname_, true);
    EXPECT_EQ(server_name_, "localhost");
    EXPECT_EQ(root_loc_.address_, "/");
    EXPECT_EQ(root_loc_.root_ , "resources/root_loc_default");
    EXPECT_NE(root_loc_.index_.find("/htmls/index.html"), root_loc_.index_.end());

    const std::set<ErrPage>::iterator &NotFoundErrPage =
            root_loc_.error_pages_.find(ErrPage("/htmls/404.html", 404));
    EXPECT_NE(NotFoundErrPage, root_loc_.error_pages_.end());
    EXPECT_EQ(NotFoundErrPage->address_, "/htmls/404.html");

    const std::set<ErrPage>::iterator &ForbiddenErrPage =
            root_loc_.error_pages_.find(ErrPage("/htmls/403.html", 403));
    EXPECT_NE(ForbiddenErrPage, root_loc_.error_pages_.end());
    EXPECT_EQ(ForbiddenErrPage->address_, "/htmls/403.html");

    EXPECT_NE(root_loc_.error_pages_.find(ErrPage("/htmls/403.html", 403)),
              root_loc_.error_pages_.end());
    EXPECT_EQ(root_loc_.return_code_ , -1);
    EXPECT_EQ(root_loc_.return_address_ , "unspecified");
}

TEST_F(ServerConfigTest, ServerConfDirectivesSuccess) {
    directives_.push_back({"listen", "8080"});
    directives_.push_back({"server_name", "example.com"});
    directives_.push_back({"client_max_body_size", "2048"});
    directives_.push_back({"error_page", "404", "/404.html"});
    directives_.push_back({"error_page", "500", "502", "503", "504", "/50x.html"});

    EXPECT_NO_THROW(CheckServerDirectives(directives_));

    EXPECT_EQ(port_, 8080);
    EXPECT_EQ(port_str_, "8080");
    EXPECT_EQ(default_index_, true);
    EXPECT_EQ(default_hostname_, false);
    EXPECT_EQ(server_name_, "localhost");
    EXPECT_EQ(server_names_.find("example.com"), server_names_.begin());
    EXPECT_EQ(root_loc_.address_, "/");
    EXPECT_EQ(root_loc_.root_ , "resources/root_loc_default");
    EXPECT_NE(root_loc_.index_.find("/htmls/index.html"), root_loc_.index_.end());

    const std::set<ErrPage>::iterator &NotFoundErrPage =
            root_loc_.error_pages_.find(ErrPage("/404.html", 404));
    EXPECT_NE(NotFoundErrPage, root_loc_.error_pages_.end());
    EXPECT_EQ(NotFoundErrPage->address_, "/404.html");

    const std::set<ErrPage>::iterator &InternalServerError =
            root_loc_.error_pages_.find(ErrPage("/50x.html", 500));
    EXPECT_NE(InternalServerError, root_loc_.error_pages_.end());
    EXPECT_EQ(InternalServerError->address_, "/50x.html");

    EXPECT_EQ(root_loc_.return_code_ , -1);
    EXPECT_EQ(root_loc_.return_address_ , "unspecified");
}

TEST_F(ServerConfigTest, ServerConfDirectivesPortOnlySuccess) {
    directives_.push_back({"listen", "8080"});

    EXPECT_NO_THROW(CheckServerDirectives(directives_));
}

TEST_F(ServerConfigTest, ServerConfDirectivesNoPortSpecifiedFail) {
    directives_.push_back({"server_name", "localhost"});
    directives_.push_back({"client_max_body_size", "2048"});
    directives_.push_back({"error_page", "404", "/404.html"});
    directives_.push_back({"error_page", "500", "502", "503", "504", "/50x.html"});

    EXPECT_THROW(CheckServerDirectives(directives_),
                 ConfigFileSyntaxError);
}

TEST_F(ServerConfigTest, ServerConfDirectivesMultiplePortSpecifiedFail) {
    directives_.push_back({"listen", "8081"});
    directives_.push_back({"listen", "8082"});
    directives_.push_back({"server_name", "localhost"});
    directives_.push_back({"client_max_body_size", "2048"});
    directives_.push_back({"error_page", "404", "/404.html"});
    directives_.push_back({"error_page", "500", "502", "503", "504", "/50x.html"});

    EXPECT_THROW(CheckServerDirectives(directives_),
                 ConfigFileSyntaxError);
}

TEST_F(ServerConfigTest, ServerConfDirectivesMultipleBodySize) {
    directives_.push_back({"listen", "8081"});
    directives_.push_back({"server_name", "localhost"});
    directives_.push_back({"client_max_body_size", "2048"});
    directives_.push_back({"client_max_body_size", "10204"});
    directives_.push_back({"error_page", "404", "/404.html"});
    directives_.push_back({"error_page", "500", "502", "503", "504", "/50x.html"});

    EXPECT_THROW(CheckServerDirectives(directives_),
                 ConfigFileSyntaxError);
}

TEST_F(ServerConfigTest, ServerConfDirectivesMultipleRoot) {
    directives_.push_back({"listen", "8081"});
    directives_.push_back({"server_name", "localhost"});
    directives_.push_back({"root", "ff/zz"});
    directives_.push_back({"root", "zz/ff"});
    directives_.push_back({"error_page", "404", "/404.html"});
    directives_.push_back({"error_page", "500", "502", "503", "504", "/50x.html"});

    EXPECT_THROW(CheckServerDirectives(directives_),
                 ConfigFileSyntaxError);
}