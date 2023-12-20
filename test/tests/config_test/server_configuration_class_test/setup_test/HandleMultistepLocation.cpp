/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    HandleMultistepLocation.cpp                        :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/17 14:11:46 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/server_configuration/ServerConfiguration.h"

class HandleMultistepLocationTest : public ::testing::Test,
                                    public ServerConfiguration {
public:
    explicit HandleMultistepLocationTest() : ServerConfiguration() {};
protected:
    Node context_1_layer_;
    Node context_2_layer_;
    Node location_root_;

    virtual void SetUp() {
        location_root_ = Node();

        location_root_.main_ = v_str({"location", "/"});
        location_root_.directives_.push_back({"listen", "8080"});
        location_root_.directives_.push_back({"server_name", "example.com"});
        location_root_.directives_.push_back({"client_max_body_size", "2048"});
        location_root_.directives_.push_back({"error_page", "404", "/404.html"});
        location_root_.directives_.push_back({"error_page", "500", "502",
                                              "503", "504", "/50x.html"});
    }
};

TEST_F(HandleMultistepLocationTest, TwoStepInServerContext) {
    context_1_layer_.main_ = v_str({"location", "/loc_5X/loc_7X" });
    context_1_layer_.directives_.push_back({"index", "index.html"});
//-------------------like if it would be right in a server context--------------
    EXPECT_NO_THROW(HandleLocationContext(context_1_layer_));

    EXPECT_EQ(locations_.size(), 1);
    EXPECT_EQ(locations_.begin(), GetRootIt());
    EXPECT_EQ(GetRoot().sublocations_.size(), 1);
    EXPECT_EQ(GetRoot().sublocations_.begin()->parent_, GetRootIt());

    Location &loc_5X = GetRoot().sublocations_.back();

    EXPECT_EQ(loc_5X.address_, "/loc_5X");
    EXPECT_EQ(loc_5X.full_address_, "/loc_5X");
    EXPECT_EQ(loc_5X.sublocations_.size(), 1);
    EXPECT_TRUE(loc_5X.ghost_);
    EXPECT_EQ(*loc_5X.parent_, GetRoot());

    Location &loc_7X = loc_5X.sublocations_.back();

    EXPECT_EQ(loc_7X.address_, "/loc_7X");
    EXPECT_EQ(loc_7X.full_address_, "/loc_5X/loc_7X");
    EXPECT_EQ(loc_7X.sublocations_.size(), 0);
    EXPECT_EQ(loc_7X.index_.size(), 1);
    EXPECT_EQ(loc_7X.index_.front(), "index.html");
    EXPECT_FALSE(loc_7X.ghost_);
    EXPECT_EQ(*loc_7X.parent_, loc_5X);
    EXPECT_EQ(*loc_7X.parent_->parent_, GetRoot());
}

TEST_F(HandleMultistepLocationTest, TwoStepInRootContext) {
    context_1_layer_.main_ = v_str({"location", "/loc_5X/loc_7X" });
    context_1_layer_.directives_.push_back({"index", "index.html"});
//-------------------like if it would be in "/" context, which is in "server"---
    location_root_.child_nodes_.push_back(context_1_layer_);
    EXPECT_NO_THROW(HandleLocationContext(location_root_));

    EXPECT_EQ(locations_.size(), 1);
    EXPECT_EQ(locations_.begin(), GetRootIt());
    EXPECT_EQ(GetRoot().sublocations_.size(), 1);
    EXPECT_EQ(GetRoot().sublocations_.begin()->parent_, GetRootIt());

    Location &loc_5X = GetRoot().sublocations_.back();

    EXPECT_EQ(loc_5X.address_, "/loc_5X");
    EXPECT_EQ(loc_5X.full_address_, "/loc_5X");
    EXPECT_EQ(loc_5X.sublocations_.size(), 1);
    EXPECT_TRUE(loc_5X.ghost_);
    EXPECT_EQ(*loc_5X.parent_, GetRoot());

    Location &loc_7X = loc_5X.sublocations_.back();

    EXPECT_EQ(loc_7X.address_, "/loc_7X");
    EXPECT_EQ(loc_7X.full_address_, "/loc_5X/loc_7X");
    EXPECT_EQ(loc_7X.sublocations_.size(), 0);
    EXPECT_EQ(loc_7X.index_.size(), 1);
    EXPECT_EQ(loc_7X.index_.front(), "index.html");
    EXPECT_FALSE(loc_7X.ghost_);
    EXPECT_EQ(*loc_7X.parent_, loc_5X);
    EXPECT_EQ(*loc_7X.parent_->parent_, GetRoot());
}

TEST_F(HandleMultistepLocationTest, TwoStepInOneStepContext) {
    context_2_layer_.main_ = v_str({"location", "/loc_1/loc_3" });
    context_2_layer_.directives_.push_back({"index", "loc_3_in_loc_1_index_X.html"});
    location_root_.child_nodes_.push_back(context_2_layer_);

    context_1_layer_.main_ = v_str({"location", "/loc_1" });
    context_1_layer_.directives_.push_back({"index", "loc_1_index.html"});
    context_1_layer_.child_nodes_.push_back(context_2_layer_);
//-------------------context "/loc_1/loc_3" in context "/loc_1" in "server"-----
    EXPECT_NO_THROW(HandleLocationContext(context_1_layer_));

    EXPECT_EQ(locations_.size(), 1);
    EXPECT_EQ(locations_.begin(), GetRootIt());
    EXPECT_EQ(GetRoot().sublocations_.size(), 1);
    EXPECT_EQ(GetRoot().sublocations_.begin()->parent_, GetRootIt());

    Location &loc_1 = GetRoot().sublocations_.back();

    EXPECT_EQ(loc_1.address_, "/loc_1");
    EXPECT_EQ(loc_1.full_address_, "/loc_1");
    EXPECT_EQ(loc_1.sublocations_.size(), 1);
    EXPECT_EQ(loc_1.index_.size(), 1);
    EXPECT_EQ(loc_1.index_.front(), "loc_1_index.html");
    EXPECT_FALSE(loc_1.ghost_);
    EXPECT_EQ(*loc_1.parent_, GetRoot());

    Location &loc_3 = loc_1.sublocations_.back();

    EXPECT_EQ(loc_3.address_, "/loc_3");
    EXPECT_EQ(loc_3.full_address_, "/loc_1/loc_3");
    EXPECT_EQ(loc_3.sublocations_.size(), 0);
    EXPECT_EQ(loc_3.index_.size(), 1);
    EXPECT_EQ(loc_3.index_.front(), "loc_3_in_loc_1_index_X.html");
    EXPECT_FALSE(loc_3.ghost_);
    EXPECT_EQ(*loc_3.parent_, loc_1);
    EXPECT_EQ(*loc_3.parent_->parent_, GetRoot());
}

