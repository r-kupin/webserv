/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerNodeTest.cpp                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/08 11:11:04 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../src/Config/Config.h"
#include "../../../src/Config/ConfigExceptions.h"

class MainNodeTest : public ::testing::Test, public Config {
public:
    explicit MainNodeTest() : Config() {};
protected:
    Node root_;
    Node server_;
    Node empty_server_;
    virtual void SetUp() {
        root_ = Node();
        root_.main_ = v_str ({"main"});

        empty_server_ = Node();
        empty_server_.main_ = v_str ({"server"});

        server_ = Node();
        server_.main_ = v_str ({"server"});
        server_.directives_.push_back(
                v_str({"server_name", "example.com" }));
        server_.directives_.push_back(
                v_str({"listen", "8182" }));
        server_.directives_.push_back(
                v_str({"root", "/not/depends/on/config" }));
        server_.directives_.push_back(
                v_str({"index", "index.html", "index.php" }));
        server_.directives_.push_back(
                v_str({"error_page", "401" , "err.html" }));
    }
};

TEST_F(MainNodeTest, ComponentsTestNoServersFail) {
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(MainNodeTest, ComponentsTest1EmptyServerFail) {
    root_.child_nodes_.push_back(empty_server_);
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(MainNodeTest, ComponentsTestOKServerWorks) {
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CreateSrvConfigs(root_));
}

TEST_F(MainNodeTest, MultipleServersWithSameName) {
    Node server_with_same_name;
    server_with_same_name.main_ = v_str ({"server"});
    server_with_same_name.directives_.push_back(
            v_str({"server_name", "example.com" }));
    server_with_same_name.directives_.push_back(
            v_str({"listen", "8183" }));
    server_with_same_name.directives_.push_back(
            v_str({"root", "/also/not/depends/on/config" }));
    server_with_same_name.directives_.push_back(
            v_str({"index", "index.html", "index.php" }));
    server_with_same_name.directives_.push_back(
            v_str({"error_page", "401" , "err.html" }));
    root_.child_nodes_.push_back(server_);
    root_.child_nodes_.push_back(server_with_same_name);
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(MainNodeTest, MultipleServersWithSamePort) {
    Node server_with_same_name;
    server_with_same_name.main_ = v_str ({"server"});
    server_with_same_name.directives_.push_back(
            v_str({"server_name", "examp1e.com" }));
    server_with_same_name.directives_.push_back(
            v_str({"listen", "8182" }));
    server_with_same_name.directives_.push_back(
            v_str({"root", "/also/not/depends/on/config" }));
    server_with_same_name.directives_.push_back(
            v_str({"index", "index.html", "index.php" }));
    server_with_same_name.directives_.push_back(
            v_str({"error_page", "401" , "err.html" }));
    root_.child_nodes_.push_back(server_);
    root_.child_nodes_.push_back(server_with_same_name);
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

// todo more tests for main node directives, and server interdependency (if
//  any) check that no configs are built if they are misconfigured