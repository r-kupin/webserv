/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSynthFoundExactTest.cpp                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/28 16:55:48 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

static Config conf_index_empty_loc("test_resources/index_order/nginx_empty_loc.conf");
class LocationSynthUtilsCheckLimitIndexPriority : public ::testing::Test, public
Server {
public:
    explicit LocationSynthUtilsCheckLimitIndexPriority() : Server(conf_index_empty_loc.getConstServers().front()) {};
};

TEST_F(LocationSynthUtilsCheckLimitIndexPriority, FindIndexToSend) {
    auto root = GetConfig().FindConstLocation("/").location_;
    auto loc_X = GetConfig().FindConstLocation("/loc_X").location_;
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;

    std::cout << *root << std::endl;
    
    EXPECT_EQ(*FindIndexToSend(root), "index_1.html");
    EXPECT_EQ(FindIndexToSend(loc_X), loc_X->index_.end());
    EXPECT_EQ(*FindIndexToSend(loc_1), "/loc_1/index_1.html");
}

TEST_F(LocationSynthUtilsCheckLimitIndexPriority, HandleExplicitIndexTEST) {
    auto root = GetConfig().FindConstLocation("/").location_;
    Location synth_root(*root);
    HandleExplicitIndex(root, synth_root);
    EXPECT_EQ(synth_root.index_.size(), 1);
    EXPECT_EQ(synth_root.index_.front(), "index_1.html");
    EXPECT_EQ(synth_root.return_code_, 200);

    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    Location synth_loc_1(*loc_1);
    HandleExplicitIndex(loc_1, synth_loc_1);
    EXPECT_EQ(synth_loc_1.index_.size(), 1);
    EXPECT_EQ(synth_loc_1.index_.front(), "/loc_1/index_1.html");
    EXPECT_EQ(synth_loc_1.return_code_, 200);

    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    Location synth_loc_3(*loc_3);
    HandleExplicitIndex(loc_3, synth_loc_3);
    EXPECT_EQ(synth_loc_3.index_.size(), 0);
    EXPECT_EQ(synth_loc_3.return_code_, 403);
}

TEST_F(LocationSynthUtilsCheckLimitIndexPriority, HandleImplicitIndexTEST) {
    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    Location synth_loc_2(*loc_2);
    HandleImplicitIndex(loc_2, synth_loc_2);
    EXPECT_EQ(synth_loc_2.index_.size(), 0);
    EXPECT_EQ(synth_loc_2.return_code_, 403);

    auto loc_4 = GetConfig().FindConstLocation("/loc_4").location_;
    Location synth_loc_4(*loc_4);
    HandleImplicitIndex(loc_4, synth_loc_4);
    EXPECT_EQ(synth_loc_4.index_.size(), 1);
    EXPECT_EQ(synth_loc_4.index_.front(), "index.html");
    EXPECT_EQ(synth_loc_4.return_code_, 200);
}

TEST_F(LocationSynthUtilsCheckLimitIndexPriority, HandleLocationNoRedirection) {
    auto loc_X = GetConfig().FindConstLocation("/loc_X").location_;
    Location synth_loc_X(*loc_X);
    SynthFoundExact(loc_X, synth_loc_X);
    EXPECT_EQ(synth_loc_X.return_code_, 404);

    auto root = GetConfig().FindConstLocation("/").location_;
    Location synth_root(*root);
    SynthFoundExact(root, synth_root);
    EXPECT_EQ(synth_root.index_.size(), 1);
    EXPECT_EQ(synth_root.index_.front(), "index_1.html");
    EXPECT_EQ(synth_root.return_code_, 200);

    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    Location synth_loc_1(*loc_1);
    SynthFoundExact(loc_1, synth_loc_1);
    EXPECT_EQ(synth_loc_1.index_.size(), 1);
    EXPECT_EQ(synth_loc_1.index_.front(), "/loc_1/index_1.html");
    EXPECT_EQ(synth_loc_1.return_code_, 200);

    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    Location synth_loc_2(*loc_2);
    SynthFoundExact(loc_2, synth_loc_2);
    EXPECT_EQ(synth_loc_2.index_.size(), 0);
    EXPECT_EQ(synth_loc_2.return_code_, 403);

    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    Location synth_loc_3(*loc_3);
    SynthFoundExact(loc_3, synth_loc_3);
    EXPECT_EQ(synth_loc_3.index_.size(), 0);
    EXPECT_EQ(synth_loc_3.return_code_, 403);

    auto loc_4 = GetConfig().FindConstLocation("/loc_4").location_;
    Location synth_loc_4(*loc_4);
    SynthFoundExact(loc_4, synth_loc_4);
    EXPECT_EQ(synth_loc_4.index_.size(), 1);
    EXPECT_EQ(synth_loc_4.index_.front(), "index.html");
    EXPECT_EQ(synth_loc_4.return_code_, 200);
}

static Config conf_index_root_redefined("test_resources/index_order/nginx_root_redefined.conf");
class LocationSynthUtilsCheckLimitIndexPriorityRootRedefined : public ::testing::Test, public
Server {
public:
    explicit LocationSynthUtilsCheckLimitIndexPriorityRootRedefined() : Server(conf_index_root_redefined.getConstServers().front()) {};
};

TEST_F(LocationSynthUtilsCheckLimitIndexPriorityRootRedefined, IndexToSend) {
    auto root = GetConfig().FindConstLocation("/").location_;

    EXPECT_EQ(*FindIndexToSend(root), "index_2.html");
}

TEST_F(LocationSynthUtilsCheckLimitIndexPriorityRootRedefined, ExplicitIndexTEST) {
    auto root = GetConfig().FindConstLocation("/").location_;

    Location synth_root(*root);
    HandleExplicitIndex(root, synth_root);
    EXPECT_EQ(synth_root.index_.size(), 1);
    EXPECT_EQ(synth_root.index_.front(), "index_2.html");
    EXPECT_EQ(synth_root.return_code_, 200);
}
