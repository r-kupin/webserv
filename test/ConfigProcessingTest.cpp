/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigProcessingTest.cpp                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/23 15:15:59 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class LocationContextTest  : public ::testing::Test, public Config {
public:
    explicit LocationContextTest() : Config() {};
protected:
    Node root_;
    Node server_;
    Node location_;
    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
        root_.main_ = v_strings ({"main"});
        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "server_name", "localhost" }));
        server_.directives_.push_back(
                v_strings({ "listen", "8080" }));
        server_.directives_.push_back(
                v_strings({ "root", "/some/where/deep/inside" }));
        server_.directives_.push_back(
                v_strings({ "index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "401" , "err.html" }));
    }
};

TEST_F(LocationContextTest, ComponentsTestLocationBlockEmpty) {
    location_.main_ = v_strings({ "location", "/" });
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LocationContextTest, ComponentsTestLocationBlockWithoutPath) {
    location_.main_ = v_strings({ "location"});
    location_.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LocationContextTest, ComponentsTestAllRootInsideLocation) {
    location_.main_ = v_strings({ "location", "/" });
    location_.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(LocationContextTest, ComponentsTestAllIndexInsideLocation) {
    location_.main_ = v_strings({"location", "/" });
    location_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

class LimitExceptContextTest  : public ::testing::Test, public Config {
public:
    explicit LimitExceptContextTest() : Config() {};
protected:
    Node root_;
    Node server_;
    Node location_;
    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
        location_ = Node();

        root_.main_ = v_strings ({"main"});
        server_.main_ = v_strings ({"server"});
        server_.directives_.push_back(
                v_strings({ "server_name", "localhost" }));
        server_.directives_.push_back(
                v_strings({ "listen", "8080" }));
        server_.directives_.push_back(
                v_strings({ "root", "/some/where/deep/inside" }));
        server_.directives_.push_back(
                v_strings({ "index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_strings({ "error_page", "401" , "err.html" }));

        location_.main_ = v_strings({"location", "/" });
        location_.directives_.push_back(v_strings({"root", "/some/where"}));
    }
};

TEST_F(LimitExceptContextTest, ComponentsTestLocationSomeRandomBlocksArePresent) {
Node random;
random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

root_.child_nodes_.push_back(server_);
EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptNoHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except" });
limit_except.directives_.push_back(v_strings({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptWrongHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "QWER" });
limit_except.directives_.push_back(v_strings({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptWrongInServerContext) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "GET", "POST" });
limit_except.directives_.push_back(v_strings({"deny", "all"}));

server_.child_nodes_.push_back(limit_except);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptCorrectHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "GET" });
limit_except.directives_.push_back(v_strings({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(LimitExceptContextTest,
       ComponentsTestLimitExceptCorrectAndWrongHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "GET", "QWER" });
limit_except.directives_.push_back(v_strings({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptAllCorrectHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
limit_except.directives_.push_back(v_strings({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptButContextIsEmpty) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest,
       ComponentsTestLimitExceptButContextLacksEssentials) {
Node random;
Node limit_except;

limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
limit_except.directives_.push_back(v_strings({"bias", "whatever"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_strings({ "random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}