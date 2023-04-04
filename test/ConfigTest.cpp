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

//TEST(ConfigTest, ConfigOpensNoSemicolon) {
//    EXPECT_THROW(Config("test_resources/corrupted_config_no_semicolon.conf"),
//                 Config::ConfigFileSyntaxError);
//}

TEST(ConfigTest, ConfigOpensNoOpenBrace) {
    EXPECT_THROW(Config("test_resources/corrupted_config_no_open.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigOpensNoCloseBrace) {
    EXPECT_THROW(Config("test_resources/correct_config_no_close_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigTest, ConfigEmpty) {
    EXPECT_NO_THROW(Config("test_resources/empty_config.conf"));
}

TEST(ConfigTest, ConfigThrowsNonExistentConf) {
    EXPECT_THROW(Config("asd"), ConfigFileNotFound);
}

