/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ParsingToolsTest.cpp                               :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/05 11:52:44 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../../src/Config/config/Config.h"

class ParsingToolsTest : public ::testing::Test, public Config {};

TEST_F(ParsingToolsTest, Parse_Directive_test) {
    std::string line = "return 200 \"Hello from loc_8 !\";";
    EXPECT_EQ(ParseDirective(line, ';'),
              v_str({"return", "200", "Hello from loc_8 !"}));

    line = "error_page 403 /403_X.html;";
    EXPECT_EQ(ParseDirective(line, ';'),
              v_str({"error_page", "403", "/403_X.html"}));

    line = "er\"ror\"_page 403 /403_X.html;";
    EXPECT_EQ(ParseDirective(line, ';'),
              v_str({"er", "ror", "_page", "403", "/403_X.html"}));

    line = "\"error_page 403 /403_X.html\";";
    EXPECT_EQ(ParseDirective(line, ';'),
              v_str({"error_page 403 /403_X.html"}));

    line = "\"error_page 403 /403_X.html;\";";
    EXPECT_EQ(ParseDirective(line, ';'),
              v_str({"error_page 403 /403_X.html;"}));

    line = "\"error_page 403 /403_X.html;\"";
    EXPECT_THROW(ParseDirective(line, ';'), ConfigException);

    line = "er\"ror\"_page 403 /403_X.html\";";
    EXPECT_THROW(ParseDirective(line, ';'), ConfigException);

    line = "\"error_page 403 /403_X.html;";
    EXPECT_THROW(ParseDirective(line, ';'), ConfigException);

    line = "error_page 403 /403_X.html\";";
    EXPECT_THROW(ParseDirective(line, ';'), ConfigException);
}
