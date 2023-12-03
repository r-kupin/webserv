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

TEST(ConfigParsingTest, ConfigOpensExistentConf) {
    EXPECT_NO_THROW(Config("test_resources/correct_config.conf"));
}

TEST(ConfigParsingTest, ConfigOpensOneLine) {
    EXPECT_NO_THROW(Config("test_resources/correct_config_in_one_line.conf"));
}

TEST(ConfigParsingTest, ConfigOpensMultipleDirectivesOneLine) {
    EXPECT_NO_THROW(Config("test_resources/correct_config_multiple_directives_one_line.conf"));
}

TEST(ConfigParsingTest, ConfigFailsEmptyBlock) {
    EXPECT_THROW(Config("test_resources/corrupted_config_empty_block.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsExcessSemicolon) {
    EXPECT_THROW(Config("test_resources/corrupted_config_excess_semicolon.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsMissingSemicolon) {
    EXPECT_THROW(Config("test_resources/corrupted_config_missing_semicolon.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsNoOpenBrace) {
    EXPECT_THROW(Config("test_resources/corrupted_config_no_open_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsNoCloseBrace) {
    EXPECT_THROW(Config("test_resources/corrupted_config_no_close_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsMultipleServersSamePort) {
    EXPECT_THROW(Config("test_resources/corrupted_config_multiple_servers_for_one_port.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsMultipleLocationsSamePath) {
    EXPECT_THROW(Config("test_resources/corrupted_config_multiple_locations_for_one_path.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigThrowsNonExistentConf) {
    EXPECT_THROW(Config("asd"), ConfigFileNotFound);
}

//TEST_F(ComponentsTest, ComponentsTestLimitExceptReturnOnly) {TODO if condition
//    Node location;
//    Node random;
//    Node limit_except;
//
//    limit_except.main_ = v_str({ "limit_except", "GET", "POST", "DELETE" });
//    limit_except.directives_.push_back(v_str({"return", "404"}));
//    location.child_nodes_.push_back(limit_except);
//
//    location.main_ = v_str({ "location", "/" });
//    location.directives_.push_back(v_str({"root", "/some/where"}));
//    empty_server_.child_nodes_.push_back(location);
//
//
//    random.main_ = v_str({ "random" });
//    empty_server_.child_nodes_.push_back(random);
//
//    empty_server_.directives_.push_back(
//            v_str({ "server_name", "localhost" }));
//    empty_server_.directives_.push_back(
//            v_str({ "listen", "8080" }));
//    empty_server_.directives_.push_back(
//            v_str({ "index", "index.html", "index.htm" }));
//    empty_server_.directives_.push_back(
//            v_str({ "error_page", "err.html" }));
//    root_.child_nodes_.push_back(empty_server_);
//    EXPECT_NO_THROW(CheckComponents(root_));
//}
