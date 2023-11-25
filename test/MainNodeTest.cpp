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
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class MainNodeTest : public ::testing::Test, public Config {
public:
    explicit MainNodeTest() : Config() {};
protected:
    Node root_;
    Node server_;
    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
        root_.main_ = v_strings ({"main"});
        server_.main_ = v_strings ({"server"});
    }
};

TEST_F(MainNodeTest, ComponentsTestNoServersFail) {
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(MainNodeTest, ComponentsTest1EmptyServerFail) {
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

