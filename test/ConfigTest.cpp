/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigTest.cpp                                     :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/03/28 03:24:12 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

TEST(ConfigTest, ConfigOpensExistentConf) {
    EXPECT_NO_THROW(Config("test_resources/correct_config.conf"));
}

TEST(ConfigTest, ConfigOpensOneLine) {
    EXPECT_NO_THROW(Config("test_resources/correct_config_in_one_line.conf"));
}

TEST(ConfigTest, ConfigOpensMultipleDirectivesOneLine) {
    EXPECT_NO_THROW(Config("test_resources/correct_config_multiple_directives_one_line.conf"));
}

TEST(ConfigTest, ConfigFailsEmptyBlock) {
    EXPECT_THROW(Config("test_resources/corrupted_config_empty_block.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigFailsExcessSemicolon) {
    EXPECT_THROW(Config("test_resources/corrupted_config_excess_semicolon.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigFailsMissingSemicolon) {
    EXPECT_THROW(Config("test_resources/corrupted_config_missing_semicolon.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigFailsNoOpenBrace) {
    EXPECT_THROW(Config("test_resources/corrupted_config_no_open_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigFailsNoCloseBrace) {
    EXPECT_THROW(Config("test_resources/corrupted_config_no_close_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigThrowsNonExistentConf) {
    EXPECT_THROW(Config("asd"), ConfigFileNotFound);
}

class ComponentsTest : public ::testing::Test, public Config {
public:
    explicit ComponentsTest() : Config() {};
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

TEST_F(ComponentsTest, ComponentsTestNoServersFail) {
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTest1EmptyServerFail) {
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTest1InsufficientServerFail) {
    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "root", "/some/where/deep/inside" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestAllPresent) {
    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "root", "/some/where/deep/inside" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(ComponentsTest, ComponentsTestLocationBlockEmpty) {
    Node location;
    location.main_ = v_strings({ "location", "/" });
    server_.child_nodes_.push_back(location);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "root", "/some/where/deep/inside" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestLocationBlockWithoutPath) {
    Node location;
    location.main_ = v_strings({ "location"});
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestAllRootInsideLocation) {
    Node location;
    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(ComponentsTest, ComponentsTestAllIndexInsideLocation) {
    Node location;
    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.child_nodes_.push_back(location);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({"root", "/some/where"}));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(ComponentsTest, ComponentsTestLocationHasNoMeaningfullDirectives) {
    Node location;
    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(
            v_strings({ "autoindex", "on"}));
    server_.child_nodes_.push_back(location);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({"root", "/some/where"}));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestLocationSomeRandomBlocksArePresent) {
    Node location;
    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);

    Node random;
    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptNoHTTPMethods) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except" });
    limit_except.directives_.push_back(v_strings({"deny", "all"}));
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptWrongHTTPMethods) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except", "QWER" });
    limit_except.directives_.push_back(v_strings({"deny", "all"}));
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptCorrectHTTPMethods) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except", "GET" });
    limit_except.directives_.push_back(v_strings({"deny", "all"}));
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptCorrectAndWrongHTTPMethods) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except", "GET", "QWER" });
    limit_except.directives_.push_back(v_strings({"deny", "all"}));
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptAllCorrectHTTPMethods) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
    limit_except.directives_.push_back(v_strings({"deny", "all"}));
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CheckComponents(root_));
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptButContextIsEmpty) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

TEST_F(ComponentsTest, ComponentsTestLimitExceptButContextLacksEssentials) {
    Node location;
    Node random;
    Node limit_except;

    limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
    limit_except.directives_.push_back(v_strings({"bias", "whatever"}));
    location.child_nodes_.push_back(limit_except);

    location.main_ = v_strings({ "location", "/" });
    location.directives_.push_back(v_strings({"root", "/some/where"}));
    server_.child_nodes_.push_back(location);


    random.main_ = v_strings({ "random" });
    server_.child_nodes_.push_back(random);

    server_.directives_.push_back(
            v_strings({ "server_name", "localhost" }));
    server_.directives_.push_back(
            v_strings({ "listen", "8080" }));
    server_.directives_.push_back(
            v_strings({ "index", "index.html", "index.htm" }));
    server_.directives_.push_back(
            v_strings({ "error_page", "err.html" }));
    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CheckComponents(root_), ConfigFileSyntaxError);
}

//TEST_F(ComponentsTest, ComponentsTestLimitExceptReturnOnly) {TODO if condition
//    Node location;
//    Node random;
//    Node limit_except;
//
//    limit_except.main_ = v_strings({ "limit_except", "GET", "POST", "DELETE" });
//    limit_except.directives_.push_back(v_strings({"return", "404"}));
//    location.child_nodes_.push_back(limit_except);
//
//    location.main_ = v_strings({ "location", "/" });
//    location.directives_.push_back(v_strings({"root", "/some/where"}));
//    server_.child_nodes_.push_back(location);
//
//
//    random.main_ = v_strings({ "random" });
//    server_.child_nodes_.push_back(random);
//
//    server_.directives_.push_back(
//            v_strings({ "server_name", "localhost" }));
//    server_.directives_.push_back(
//            v_strings({ "listen", "8080" }));
//    server_.directives_.push_back(
//            v_strings({ "index", "index.html", "index.htm" }));
//    server_.directives_.push_back(
//            v_strings({ "error_page", "err.html" }));
//    root_.child_nodes_.push_back(server_);
//    EXPECT_NO_THROW(CheckComponents(root_));
//}
