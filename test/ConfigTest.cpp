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

TEST(ConfigTest, ConfigOpensMixedUp) {
    EXPECT_NO_THROW(Config("test_resources/correct_config_mixed_up.conf"));
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

//TEST(ConfigTest, ConfigEmpty) {
//    EXPECT_NO_THROW(Config("test_resources/empty_config.conf"));
//}
