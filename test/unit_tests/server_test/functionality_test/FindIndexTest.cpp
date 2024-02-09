/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    FindIndexTest.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/04 15:30:03 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../ConfigFiles.h"

class FindIndexTosendTest : public ::testing::Test, public Server {
public:
    explicit FindIndexTosendTest()
    : Server(simple_index_check.getConstServers().front()) {};
};

TEST_F(FindIndexTosendTest, FindIndexToSendInDefined) {
    auto root = GetConfig().FindConstLocation("/").location_;
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    auto loc_X = GetConfig().FindConstLocation("/loc_X").location_;

    EXPECT_EQ(FindIndexToSend(root, ""),
              "test_resources/index_order/www/index_1.html");
    EXPECT_EQ(FindIndexToSend(loc_1, ""),
              "test_resources/index_order/www/loc_4/index.html");
    EXPECT_EQ(FindIndexToSend(loc_2, ""),
              "test_resources/index_order/www/loc_2/index_2.html");
    EXPECT_EQ(FindIndexToSend(loc_3, ""),
              "test_resources/index_order/www/loc_3/index_X.html");
//    works fine with, cause .../loc_X/ doesn't exist anyways:
    EXPECT_EQ(FindIndexToSend(loc_X, ""),
              "test_resources/index_order/www/loc_X/index_2.html");
//    should be:
//    EXPECT_EQ(FindIndexToSend(loc_X, ""),
//              "test_resources/index_order/www/loc_X/index_X.html");
}

TEST_F(FindIndexTosendTest, FindIndexToSendInNotDefined) {
    const ServerConfiguration::LocConstSearchResult &result =
            GetConfig().FindConstLocation("/loc_4");

    EXPECT_EQ(FindIndexToSend(result.location_, result.leftower_address_),
              "test_resources/index_order/www/loc_4/index_2.html");
}

class FindIndexToSendButRootRedefined : public ::testing::Test, public
Server {
public:
    explicit FindIndexToSendButRootRedefined()
    : Server(conf_index_root_redefined.getConstServers().front()) {};
};

TEST_F(FindIndexToSendButRootRedefined, FindIndexToSendRoot) {
    auto root = GetConfig().FindConstLocation("/").location_;

    EXPECT_EQ(FindIndexToSend(root, ""),
              "test_resources/index_order/www/index_2.html");
}

TEST_F(FindIndexToSendButRootRedefined, FindIndexToSendNotDefined) {
    const ServerConfiguration::LocConstSearchResult &result_X =
            GetConfig().FindConstLocation("/X");
    const ServerConfiguration::LocConstSearchResult &result_1 =
            GetConfig().FindConstLocation("/loc_1");
    const ServerConfiguration::LocConstSearchResult &result_file_as_directory =
            GetConfig().FindConstLocation("/loc_1/index_1.html");
    const ServerConfiguration::LocConstSearchResult &result_loc_5_file =
            GetConfig().FindConstLocation("/loc_5");

    EXPECT_EQ(FindIndexToSend(result_X.location_, result_X.leftower_address_),
              "test_resources/index_order/www/X/index_2.html");

    EXPECT_EQ(FindIndexToSend( result_1.location_, result_1.leftower_address_),
              "test_resources/index_order/www/loc_1/index_2.html");

    EXPECT_EQ(FindIndexToSend(result_file_as_directory.location_,
                              result_file_as_directory.leftower_address_),
              "test_resources/index_order/www/loc_1/index_1.html/index_2.html");

    EXPECT_EQ(FindIndexToSend(result_loc_5_file.location_,
                              result_loc_5_file.leftower_address_),
              "test_resources/index_order/www/loc_5/index_2.html");
}

