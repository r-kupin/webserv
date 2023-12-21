/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ConfigSetupFromFileTest.cpp                        :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/20 11:34:39 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../../src/Config/config/Config.h"

class ConfigSetupFromFileSimpleTest : public ::testing::Test, public Config {
public:
    ConfigSetupFromFileSimpleTest() : Config("test_resources/simple/nginx.conf") {};
protected:
    std::string uri_;
};

TEST_F(ConfigSetupFromFileSimpleTest, TestAllComponents) {
    const ServerConfiguration & current = getConstServers().front();

    EXPECT_EQ(current.port_, 4280);
    EXPECT_EQ(current.port_str_, "4280");
    EXPECT_EQ(current.locations_.size(), 1);

    const Location & root = current.GetConstRoot();
    EXPECT_EQ(root.address_, "/");
    EXPECT_TRUE(root.error_pages_.empty());
    EXPECT_EQ(root.index_.size(), 1);
    EXPECT_EQ(root.index_.front(), "root_index.html");
    EXPECT_EQ(root.root_, "test_resources/simple/www");
    EXPECT_FALSE(root.ghost_);
    EXPECT_EQ(root.sublocations_.size(), 7);

    auto it = root.sublocations_.rbegin();
    const Location & loc_1X = *it;
    EXPECT_EQ(loc_1X.address_, "/loc_1X");
    EXPECT_EQ(*loc_1X.parent_, root);

    it++;
    const Location & loc_2X = *it;
    EXPECT_EQ(loc_2X.address_, "/loc_2X");
    EXPECT_EQ(*loc_2X.parent_, root);

    it++;
    const Location & loc_1 = *it;
    EXPECT_EQ(loc_1.address_, "/loc_1");
    EXPECT_EQ(*loc_1.parent_, root);

    it++;
    const Location & loc_2 = *it;
    EXPECT_EQ(loc_2.address_, "/loc_2");
    EXPECT_EQ(*loc_2.parent_, root);

    it++;
    const Location & loc_3 = *it;
    EXPECT_EQ(loc_3.address_, "/loc_3");
    EXPECT_EQ(*loc_3.parent_, root);

    it++;
    const Location & loc_4 = *it;
    EXPECT_EQ(loc_4.address_, "/loc_4");
    EXPECT_EQ(*loc_4.parent_, root);

    it++;
    const Location & loc_5 = *it;
    EXPECT_EQ(loc_5.address_, "/loc_5");
    EXPECT_EQ(*loc_5.parent_, root);
}

class ConfigSetupFromFileNestedTest : public ::testing::Test, public Config {
public:
    ConfigSetupFromFileNestedTest() : Config("test_resources/nested_locations/nginx.conf") {};
protected:
    std::string uri_;
};

TEST_F(ConfigSetupFromFileNestedTest, TestAllComponents) {
    const ServerConfiguration & current = getConstServers().front();

    EXPECT_EQ(current.port_, 4280);
    EXPECT_EQ(current.port_str_, "4280");
    EXPECT_EQ(current.locations_.size(), 1);
//-------------------root-------------------------------------------------------
    const Location & root = current.GetConstRoot();
    EXPECT_EQ(root.address_, "/");
    EXPECT_EQ(root.error_pages_.size(), 2);
    EXPECT_EQ(root.index_.front(), "root_index.html");
    EXPECT_EQ(root.root_, "test_resources/nested_locations/www");
    EXPECT_FALSE(root.ghost_);
    EXPECT_EQ(root.sublocations_.size(), 3);
//-------------------in "/"-----------------------------------------------------
    auto it = root.sublocations_.rbegin();
    const Location & loc_1 = *it;
    EXPECT_EQ(loc_1.address_, "/loc_1");
    EXPECT_EQ(*loc_1.parent_, root);
    EXPECT_EQ(*loc_1.index_.begin(), "loc_1_index.html");
    EXPECT_EQ(loc_1.error_pages_.size(), 2);
    EXPECT_EQ(loc_1.sublocations_.size(), 2);
    EXPECT_FALSE(loc_1.ghost_);

    it++;
    const Location & loc_0X = *it;
    EXPECT_EQ(loc_0X.address_, "/loc_0X");
    EXPECT_EQ(*loc_0X.parent_, root);
    EXPECT_EQ(*loc_0X.limit_except_.except_.begin(), GET);
    EXPECT_TRUE(loc_0X.sublocations_.empty());
    EXPECT_FALSE(loc_0X.ghost_);

    it++;
    const Location & loc_5 = *it;
    EXPECT_EQ(loc_5.address_, "/loc_5");
    EXPECT_EQ(*loc_5.parent_, root);
    EXPECT_EQ(loc_5.sublocations_.size(), 1);
    EXPECT_TRUE(loc_5.ghost_);
//-------------------in "/loc_1"------------------------------------------------
    const Location & loc_3 = loc_1.sublocations_.back();
    EXPECT_EQ(loc_3.address_, "/loc_3");
    EXPECT_EQ(loc_3.full_address_, "/loc_1/loc_3");
    EXPECT_EQ(*loc_3.parent_, loc_1);
    EXPECT_EQ(*loc_3.parent_->parent_, root);
    EXPECT_EQ(*loc_3.index_.begin(), "loc_3_in_loc_1_index_X.html");
    EXPECT_EQ(loc_3.error_pages_.size(), 2);
    EXPECT_EQ(loc_3.sublocations_.size(), 0);
    EXPECT_FALSE(loc_3.ghost_);

    const Location & loc_2 = loc_1.sublocations_.front();
    EXPECT_EQ(loc_2.address_, "/loc_2");
    EXPECT_EQ(loc_2.full_address_, "/loc_1/loc_2");
    EXPECT_EQ(*loc_2.parent_, loc_1);
    EXPECT_EQ(*loc_2.parent_->parent_, root);
    EXPECT_EQ(*loc_2.index_.begin(), "loc_2_in_loc_1_index.html");
    EXPECT_EQ(loc_2.error_pages_.size(), 2);
    EXPECT_EQ(loc_2.sublocations_.size(), 0);
    EXPECT_FALSE(loc_2.ghost_);
//-------------------in "/loc_5"------------------------------------------------
    const Location & loc_7 = *loc_5.sublocations_.begin();
    EXPECT_EQ(loc_7.address_, "/loc_7");
    EXPECT_EQ(loc_7.full_address_, "/loc_5/loc_7");
    EXPECT_EQ(*loc_7.parent_, loc_5);
    EXPECT_EQ(*loc_7.parent_->parent_, root);
    EXPECT_EQ(*loc_7.index_.begin(), "index.html");
    EXPECT_TRUE(loc_7.sublocations_.empty());
    EXPECT_FALSE(loc_7.ghost_);
}
