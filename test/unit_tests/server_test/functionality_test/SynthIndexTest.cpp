/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SynthIndexTest.cpp                                 :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/28 16:55:48 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../ConfigFiles.h"

class SynthIndexTest_SimpleIndexCheck : public ::testing::Test, public Server {
public:
    explicit SynthIndexTest_SimpleIndexCheck()
    : Server(simple_index_check.getConstServers().front()) {};
};

TEST_F(SynthIndexTest_SimpleIndexCheck, Root) {
    Srch_c_Res res = GetConfig().FindConstLocation("/");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/index_1.html");
}

TEST_F(SynthIndexTest_SimpleIndexCheck, Loc_1) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_1");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/loc_4/index.html");
}

TEST_F(SynthIndexTest_SimpleIndexCheck, Loc_2) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_2");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 403);
    EXPECT_TRUE(synth.body_file_.empty());
}

TEST_F(SynthIndexTest_SimpleIndexCheck, Loc_3) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_3");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 403);
    EXPECT_TRUE(synth.body_file_.empty());
}

TEST_F(SynthIndexTest_SimpleIndexCheck, Loc_4) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_4");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 403);
    EXPECT_TRUE(synth.body_file_.empty());
}

TEST_F(SynthIndexTest_SimpleIndexCheck, Loc_X) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_X");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, NOTHING);

    EXPECT_EQ(synth.return_code_, 404);
    EXPECT_TRUE(synth.body_file_.empty());
}

class SynthIndexCheck_RootRedefined : public ::testing::Test, public Server {
public:
    explicit SynthIndexCheck_RootRedefined()
    : Server(conf_index_root_redefined.getConstServers().front()) {};
};

TEST_F(SynthIndexCheck_RootRedefined, Root) {
    Srch_c_Res res = GetConfig().FindConstLocation("/");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/index_2.html");
}

TEST_F(SynthIndexCheck_RootRedefined, Loc_1) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_1");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, DIRECTORY);

    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/loc_1/index_2.html");
}

TEST_F(SynthIndexCheck_RootRedefined, Loc_1_index_1) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_1/index_1.html");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, FILE);

    EXPECT_EQ(synth.return_code_, 404);
    EXPECT_TRUE(synth.body_file_.empty());
}

TEST_F(SynthIndexCheck_RootRedefined, Loc_5) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_5");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, FILE);

    EXPECT_EQ(synth.return_code_, 404);
    EXPECT_TRUE(synth.body_file_.empty());
}

TEST_F(SynthIndexCheck_RootRedefined, Loc_X) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_X");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthIndex(synth, res, NOTHING);

    EXPECT_EQ(synth.return_code_, 404);
    EXPECT_TRUE(synth.body_file_.empty());
}
