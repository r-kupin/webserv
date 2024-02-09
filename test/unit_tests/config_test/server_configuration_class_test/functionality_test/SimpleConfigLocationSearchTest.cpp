/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SimpleConfigLocationSearchTest.cpp                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 14:34:36 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../../src/Config/config/Config.h"

class SimpleConfigLocationSearchTest : public ::testing::Test, public Config {
public:
    SimpleConfigLocationSearchTest() : Config("test_resources/simple/nginx.conf") {};
protected:
    std::string uri_;
};

TEST_F(SimpleConfigLocationSearchTest, FindRootLocation) {
    uri_ = "/";
    ServerConfiguration &config = GetServers().front();
    const Srch_c_Res &result =
            config.FindConstLocation(uri_);

    const Location & root = *result.location_;
    EXPECT_EQ(root, config.GetConstRoot());
    EXPECT_EQ(root.address_, "/");
    EXPECT_TRUE(root.error_pages_.empty());
    EXPECT_EQ(root.own_index_.size(), 1);
    EXPECT_EQ(root.own_index_.front(), "root_index.html");
    EXPECT_EQ(root.root_, "test_resources/simple/www");
    EXPECT_FALSE(root.ghost_);
    EXPECT_EQ(root.sublocations_.size(), 8);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}

TEST_F(SimpleConfigLocationSearchTest, FindDefinedLocation) {
    uri_ = "/loc_1";

    ServerConfiguration &config = GetServers().front();
    const Srch_c_Res &result =
            config.FindConstLocation(uri_);

    EXPECT_EQ(result.location_->address_, uri_);
    EXPECT_EQ(*result.location_->parent_, config.GetConstRoot());
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedRootSubLocation) {
    uri_ = "/XXX";
    ServerConfiguration &config = GetServers().front();
    const Srch_c_Res &result =
            config.FindConstLocation(uri_);

    EXPECT_EQ(*result.location_, config.GetConstRoot());
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_address_, "/XXX");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedSubLocationOfNonRoot) {
    uri_ = "/loc_1/XXX";

    ServerConfiguration &config = GetServers().front();
    const Srch_c_Res &result =
            config.FindConstLocation(uri_);

    EXPECT_EQ(result.location_->address_, "/loc_1");
    EXPECT_EQ(*result.location_->parent_, config.GetConstRoot());
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_address_, "/XXX");
}