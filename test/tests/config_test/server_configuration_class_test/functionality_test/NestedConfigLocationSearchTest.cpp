/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    NestedConfigLocationSearchTest.cpp                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 14:35:27 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>

#include "../../../../../src/Config/config/Config.h"

class NestedConfigLocationSearchTest : public ::testing::Test, public Config {
public:
    NestedConfigLocationSearchTest(): Config("test_resources/nested_locations/nginx.conf") {};
protected:
    std::string uri_;
};

//-------------------root-------------------------------------------------------
TEST_F(NestedConfigLocationSearchTest, FindRootLocation) {
    uri_ = "/";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & root = *result.location_;
    EXPECT_EQ(root.address_, "/");
    EXPECT_EQ(root.error_pages_.size(), 2);
    EXPECT_EQ(root.index_.front(), "root_index.html");
    EXPECT_EQ(root.root_, "test_resources/nested_locations/www");
    EXPECT_FALSE(root.ghost_);
    EXPECT_EQ(root.sublocations_.size(), 3);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}
//-------------------in "/" defined---------------------------------------------
TEST_F(NestedConfigLocationSearchTest, FindDefinedLocation) {
    uri_ = "/loc_1";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & loc_1 = *result.location_;
    std::cout << *config.GetRootIt() << std::endl;
    EXPECT_EQ(loc_1.address_, "/loc_1");
    EXPECT_EQ(*loc_1.parent_, config.GetRoot());
    EXPECT_EQ(*loc_1.index_.begin(), "loc_1_index.html");
    EXPECT_EQ(loc_1.error_pages_.size(), 2);
    EXPECT_EQ(loc_1.sublocations_.size(), 2);
    EXPECT_FALSE(loc_1.ghost_);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}

TEST_F(NestedConfigLocationSearchTest, DefinedLocationInTwiceNestedRoot) {
    uri_ = "/loc_0X";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & loc_0X = *result.location_;
    EXPECT_EQ(loc_0X.address_, "/loc_0X");
    EXPECT_EQ(*loc_0X.parent_, config.GetRoot());
    EXPECT_EQ(*loc_0X.limit_except_.except_.begin(), GET);
    EXPECT_TRUE(loc_0X.sublocations_.empty());
    EXPECT_FALSE(loc_0X.ghost_);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}

TEST_F(NestedConfigLocationSearchTest, LocationIsDefinedInServerContext) {
    uri_ = "/loc_5";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & loc_5 = *result.location_;
    EXPECT_EQ(loc_5.address_, "/loc_5");
    EXPECT_EQ(*loc_5.parent_, config.GetRoot());
    EXPECT_EQ(loc_5.sublocations_.size(), 1);
    EXPECT_TRUE(loc_5.ghost_);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}
//-------------------in "/" not defined-----------------------------------------
TEST_F(NestedConfigLocationSearchTest, FindUnDefinedRootSubLocation) {
    uri_ = "/XXX";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & xxx = *result.location_;
    EXPECT_EQ(xxx, config.GetRoot());

    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_address_, "/XXX");
}
//-------------------in "/loc_1" defined----------------------------------------
TEST_F(NestedConfigLocationSearchTest, Nested_loc_1_loc_3) {
    uri_ = "/loc_1/loc_3";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & loc_3 = *result.location_;
    EXPECT_EQ(loc_3.address_, "/loc_3");
    EXPECT_EQ(loc_3.full_address_, "/loc_1/loc_3");
    EXPECT_EQ(loc_3.parent_->address_, "/loc_1");
    EXPECT_EQ(loc_3.parent_->parent_->address_, "/");
    EXPECT_EQ(*loc_3.index_.begin(), "loc_3_in_loc_1_index_X.html");
    EXPECT_EQ(loc_3.error_pages_.size(), 2);
    EXPECT_EQ(loc_3.sublocations_.size(), 0);
    EXPECT_FALSE(loc_3.ghost_);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}

TEST_F(NestedConfigLocationSearchTest, Nested_loc_1_loc_2) {
    uri_ = "/loc_1/loc_2";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & loc_2 = *result.location_;
    EXPECT_EQ(loc_2.address_, "/loc_2");
    EXPECT_EQ(loc_2.full_address_, "/loc_1/loc_2");
    EXPECT_EQ(loc_2.parent_->address_, "/loc_1");
    EXPECT_EQ(loc_2.parent_->parent_->address_, "/");
    EXPECT_EQ(*loc_2.index_.begin(), "loc_2_in_loc_1_index.html");
    EXPECT_EQ(loc_2.error_pages_.size(), 2);
    EXPECT_EQ(loc_2.sublocations_.size(), 0);
    EXPECT_FALSE(loc_2.ghost_);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}
//-------------------in "/loc_1" not defined------------------------------------
TEST_F(NestedConfigLocationSearchTest, FindUnDefinedSubLocationOfNonRoot) {
    uri_ = "/loc_1/XXX";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & xxx = *result.location_;
    EXPECT_EQ(xxx.address_, "/loc_1");
    EXPECT_EQ(*xxx.parent_, config.GetRoot());
    EXPECT_EQ(*xxx.index_.begin(), "loc_1_index.html");
    EXPECT_EQ(xxx.error_pages_.size(), 2);
    EXPECT_EQ(xxx.sublocations_.size(), 2);
    EXPECT_FALSE(xxx.ghost_);

    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_address_, "/XXX");
}
//-------------------in "/loc_5" defined----------------------------------------
TEST_F(NestedConfigLocationSearchTest, OneStepNested) {
    uri_ = "/loc_5/loc_7";

    ServerConfiguration &config = getServers().front();
    const ServerConfiguration::LocSearchResult &result =
            config.FindLocation(uri_);
    const Location & loc_7 = *result.location_;
    EXPECT_EQ(loc_7.address_, "/loc_7");
    EXPECT_EQ(loc_7.full_address_, "/loc_5/loc_7");
    EXPECT_EQ(loc_7.parent_->address_, "/loc_5");
    EXPECT_EQ(loc_7.parent_->parent_->address_, "/");
    EXPECT_EQ(*loc_7.index_.begin(), "index.html");
    EXPECT_TRUE(loc_7.sublocations_.empty());
    EXPECT_FALSE(loc_7.ghost_);

    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_address_, "");
}

