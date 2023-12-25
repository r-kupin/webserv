/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSynthUtilsTest.cpp                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:55:35 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

class LocationSynthUtilsCheckFilesystem : public ::testing::Test, public Server {
public:
    explicit LocationSynthUtilsCheckFilesystem() : Server() {};
protected:
    std::string def_res_address_ = "test_resources/nested_locations/www";
};

TEST_F(LocationSynthUtilsCheckFilesystem, CheckFilesystemExist) {
    EXPECT_TRUE(CheckFilesystem("/", def_res_address_));
    EXPECT_TRUE(CheckFilesystem("/loc_1", def_res_address_));
    EXPECT_TRUE(CheckFilesystem("/loc_1/loc_3/loc_3_in_loc_1_404.html",
                                                            def_res_address_));
}

TEST_F(LocationSynthUtilsCheckFilesystem, CheckFilesystemNotExist) {
    EXPECT_FALSE(CheckFilesystem("/X", def_res_address_));
    EXPECT_FALSE(CheckFilesystem("/loc_1/X", def_res_address_));
    EXPECT_FALSE(CheckFilesystem("/loc_1/loc_4/X",def_res_address_));
}

// todo: more tests with ip addressess

static Config conf_get("test_resources/limit_except/nginx_get.conf");

class LocationSynthUtilsCheckLimitExceptGet : public ::testing::Test, public Server {
public:
    LocationSynthUtilsCheckLimitExceptGet() : Server(conf_get.getConstServers().front()) {};
protected:
};

TEST_F(LocationSynthUtilsCheckLimitExceptGet, GET) {
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    auto loc_1_1 = GetConfig().FindConstLocation("/loc_1/loc_1_1").location_;

    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    auto loc_2_2 = GetConfig().FindConstLocation("/loc_2/loc_2_2").location_;

    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    auto loc_3_3 = GetConfig().FindConstLocation("/loc_3/loc_3_3").location_;

    EXPECT_FALSE(AccessForbudden(loc_1, GET));
    EXPECT_FALSE(AccessForbudden(loc_1_1, GET));
    EXPECT_FALSE(AccessForbudden(loc_2, GET));
    EXPECT_FALSE(AccessForbudden(loc_2_2, GET));
    EXPECT_FALSE(AccessForbudden(loc_3, GET));
    EXPECT_FALSE(AccessForbudden(loc_3_3, GET));

    EXPECT_TRUE(AccessForbudden(loc_1, POST));
    EXPECT_TRUE(AccessForbudden(loc_1_1, POST));
    EXPECT_TRUE(AccessForbudden(loc_2, POST));
    EXPECT_TRUE(AccessForbudden(loc_2_2, POST));
    EXPECT_TRUE(AccessForbudden(loc_3, POST));
    EXPECT_TRUE(AccessForbudden(loc_3_3, POST));
}

static Config conf_post("test_resources/limit_except/nginx_post.conf");

class LocationSynthUtilsCheckLimitExceptPost : public ::testing::Test, public
        Server {
public:
    LocationSynthUtilsCheckLimitExceptPost() : Server(conf_post.getConstServers().front()) {};
protected:
};

TEST_F(LocationSynthUtilsCheckLimitExceptPost, POST) {
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    auto loc_1_1 = GetConfig().FindConstLocation("/loc_1/loc_1_1").location_;

    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    auto loc_2_2 = GetConfig().FindConstLocation("/loc_2/loc_2_2").location_;

    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    auto loc_3_3 = GetConfig().FindConstLocation("/loc_3/loc_3_3").location_;

    EXPECT_TRUE(AccessForbudden(loc_1, GET));
    EXPECT_TRUE(AccessForbudden(loc_1_1, GET));
    EXPECT_TRUE(AccessForbudden(loc_2, GET));
    EXPECT_TRUE(AccessForbudden(loc_2_2, GET));
    EXPECT_TRUE(AccessForbudden(loc_3, GET));
    EXPECT_TRUE(AccessForbudden(loc_3_3, GET));

    EXPECT_TRUE(AccessForbudden(loc_1, POST));
    EXPECT_TRUE(AccessForbudden(loc_1_1, POST));
    EXPECT_TRUE(AccessForbudden(loc_2, POST));
    EXPECT_TRUE(AccessForbudden(loc_2_2, POST));
    EXPECT_TRUE(AccessForbudden(loc_3, POST));
    EXPECT_TRUE(AccessForbudden(loc_3_3, POST));
}