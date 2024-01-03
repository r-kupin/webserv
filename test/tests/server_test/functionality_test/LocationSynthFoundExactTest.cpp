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
class FindIndexTosendTest : public ::testing::Test, public
Server {
public:
    explicit FindIndexTosendTest() : Server(conf_index_empty_loc.getConstServers().front()) {};
};

//    todo: move to FindConstLocation test suite
//    EXPECT_EQ(loc_X, GetConfig().FindConstLocation("/loc_X/").location_);
//    EXPECT_EQ(loc_1, GetConfig().FindConstLocation("/loc_1/").location_);
//    EXPECT_EQ(loc_2, GetConfig().FindConstLocation("/loc_2/").location_);
//    EXPECT_EQ(loc_3, GetConfig().FindConstLocation("/loc_3/").location_);
//    EXPECT_EQ(loc_4, GetConfig().FindConstLocation("/loc_4/").location_);

TEST_F(FindIndexTosendTest, FindIndexToSendInDefined) {
    auto root = GetConfig().FindConstLocation("/").location_;
    auto loc_X = GetConfig().FindConstLocation("/loc_X").location_;
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;

    EXPECT_EQ(FindIndexToSend(root, ""),
              "test_resources/index_order/www/index_1.html");
    EXPECT_EQ(FindIndexToSend(loc_X, ""), "");
    EXPECT_EQ(FindIndexToSend(loc_1, ""),
              "test_resources/index_order/www/loc_4/index.html");
    EXPECT_EQ(FindIndexToSend(loc_2, ""), "");
    EXPECT_EQ(FindIndexToSend(loc_3, ""), "");
}

TEST_F(FindIndexTosendTest, FindIndexToSendInNotDefined) {
    const ServerConfiguration::LocConstSearchResult &result =
                            GetConfig().FindConstLocation("/loc_4");
    
    EXPECT_EQ(FindIndexToSend(
            result.location_, result.leftower_address_), "");
}

static Config conf_index_root_redefined("test_resources/index_order/nginx_root_redefined.conf");
class FindIndexToSendButRootRedefined : public ::testing::Test, public
Server {
public:
    explicit FindIndexToSendButRootRedefined() : Server(conf_index_root_redefined.getConstServers().front()) {};
};

TEST_F(FindIndexToSendButRootRedefined, FindIndexToSendRoot) {
    auto root = GetConfig().FindConstLocation("/").location_;

    EXPECT_EQ(FindIndexToSend(root), "test_resources/index_order/www/index_2.html");
}

TEST_F(FindIndexToSendButRootRedefined, FindIndexToSendNotDefined) {
    const ServerConfiguration::LocConstSearchResult &result_X =
            GetConfig().FindConstLocation("/loc_X");
    const ServerConfiguration::LocConstSearchResult &result_1 =
            GetConfig().FindConstLocation("/loc_1");
    const ServerConfiguration::LocConstSearchResult &result_file_as_directory =
            GetConfig().FindConstLocation("/loc_1/index_1.html");

    EXPECT_EQ(FindIndexToSend(result_X.location_, result_X.leftower_address_),
              "");

    EXPECT_EQ(FindIndexToSend( result_1.location_, result_1.leftower_address_),
              "test_resources/index_order/www/loc_1/index_2.html");

    EXPECT_EQ(FindIndexToSend(result_file_as_directory.location_,
                              result_file_as_directory.leftower_address_),
              "");
}













class HandleExplicitIndexTest : public FindIndexTosendTest {};

TEST_F(HandleExplicitIndexTest, HandleExplicitIndexTEST) {
//    auto root = GetConfig().FindConstLocation("/").location_;
//    Location synth_root(*root);
//    HandleExplicitIndex(root, synth_root);
//    EXPECT_EQ(synth_root.own_index_.size(), 1);
//    EXPECT_EQ(synth_root.own_index_.front(), "index_1.html");
//    EXPECT_EQ(synth_root.return_code_, 200);
//
//    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
//    Location synth_loc_1(*loc_1);
//    HandleExplicitIndex(loc_1, synth_loc_1);
//    EXPECT_EQ(synth_loc_1.own_index_.size(), 1);
//    EXPECT_EQ(synth_loc_1.own_index_.front(), "/loc_1/index_1.html");
//    EXPECT_EQ(synth_loc_1.return_code_, 200);
//
//    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
//    Location synth_loc_3(*loc_3);
//    HandleExplicitIndex(loc_3, synth_loc_3);
//    EXPECT_EQ(synth_loc_3.own_index_.size(), 0);
//    EXPECT_EQ(synth_loc_3.return_code_, 403);
}

TEST_F(HandleExplicitIndexTest, HandleImplicitIndexTEST) {
//    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
//    Location synth_loc_2(*loc_2);
//    HandleImplicitIndex(loc_2, synth_loc_2);
//    EXPECT_EQ(synth_loc_2.own_index_.size(), 0);
//    EXPECT_EQ(synth_loc_2.return_code_, 403);
//
//    auto loc_4 = GetConfig().FindConstLocation("/loc_4").location_;
//    Location synth_loc_4(*loc_4);
//    HandleImplicitIndex(loc_4, synth_loc_4);
//    EXPECT_EQ(synth_loc_4.own_index_.size(), 1);
//    EXPECT_EQ(synth_loc_4.own_index_.front(), "index.html");
//    EXPECT_EQ(synth_loc_4.return_code_, 200);
}

TEST_F(HandleExplicitIndexTest, HandleLocationNoRedirection) {
//    auto loc_X = GetConfig().FindConstLocation("/loc_X").location_;
//    Location synth_loc_X(*loc_X);
//    SynthFoundExact(loc_X, synth_loc_X, false);
//    EXPECT_EQ(synth_loc_X.return_code_, 404);
//
//    auto root = GetConfig().FindConstLocation("/").location_;
//    Location synth_root(*root);
//    SynthFoundExact(root, synth_root, false);
//    EXPECT_EQ(synth_root.index_.size(), 1);
//    EXPECT_EQ(synth_root.index_.front(), "index_1.html");
//    EXPECT_EQ(synth_root.return_code_, 200);
//
//    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
//    Location synth_loc_1(*loc_1);
//    SynthFoundExact(loc_1, synth_loc_1, false);
//    EXPECT_EQ(synth_loc_1.index_.size(), 1);
//    EXPECT_EQ(synth_loc_1.index_.front(), "/loc_1/index_1.html");
//    EXPECT_EQ(synth_loc_1.return_code_, 200);
//
//    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
//    Location synth_loc_2(*loc_2);
//    SynthFoundExact(loc_2, synth_loc_2, false);
//    EXPECT_EQ(synth_loc_2.index_.size(), 0);
//    EXPECT_EQ(synth_loc_2.return_code_, 403);
//
//    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
//    Location synth_loc_3(*loc_3);
//    SynthFoundExact(loc_3, synth_loc_3, false);
//    EXPECT_EQ(synth_loc_3.index_.size(), 0);
//    EXPECT_EQ(synth_loc_3.return_code_, 403);
//
//    auto loc_4 = GetConfig().FindConstLocation("/loc_4").location_;
//    Location synth_loc_4(*loc_4);
//    SynthFoundExact(loc_4, synth_loc_4, false);
//    EXPECT_EQ(synth_loc_4.index_.size(), 1);
//    EXPECT_EQ(synth_loc_4.index_.front(), "index.html");
//    EXPECT_EQ(synth_loc_4.return_code_, 200);
}


class HandleExplicitIndexRootRedefinedTest : public FindIndexToSendButRootRedefined{};

TEST_F(HandleExplicitIndexRootRedefinedTest, ExplicitIndexTEST) {
//    auto root = GetConfig().FindConstLocation("/").location_;
//
//    Location synth_root(*root);
//    HandleExplicitIndex(root, synth_root);
//    EXPECT_EQ(synth_root.own_index_.size(), 1);
//    EXPECT_EQ(synth_root.own_index_.front(), "index_2.html");
//    EXPECT_EQ(synth_root.return_code_, 200);
}
