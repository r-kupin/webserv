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
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Config/config/ConfigExceptions.h"

TEST(ConfigParsingTest, ConfigOpensExistentConf) {
    EXPECT_NO_THROW(Config("test_resources/test_config_samples/correct_config.conf"));
}

TEST(ConfigParsingTest, ConfigOpensOneLine) {
    EXPECT_NO_THROW(Config("test_resources/test_config_samples/correct_config_in_one_line.conf"));
}

TEST(ConfigParsingTest, ConfigOpensMultipleDirectivesOneLine) {
    EXPECT_NO_THROW(Config("test_resources/test_config_samples/correct_config_multiple_directives_one_line.conf"));
}

TEST(ConfigParsingTest, ConfigFailsEmptyBlock) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_empty_block.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsExcessSemicolon) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_excess_semicolon.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsMissingSemicolon) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_missing_semicolon.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsNoOpenBrace) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_no_open_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsNoCloseBrace) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_no_close_brace.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsMultipleServersSamePort) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_multiple_servers_for_one_port.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigThrowsNonExistentConf) {
    EXPECT_THROW(Config("asd"), ConfigFileNotFound);
}

TEST(ConfigParsingTest, ConfigFailsMultipleLocationsSamePath) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_multiple_locations_for_one_path.conf"),
                 ConfigFileSyntaxError);
}

TEST(ConfigParsingTest, ConfigFailsLocationWithWrongPath) {
    EXPECT_THROW(Config("test_resources/test_config_samples/corrupted_config_sublocation_without_absolute_path.conf"),
                 ConfigFileSyntaxError);
}
