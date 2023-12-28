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
    EXPECT_TRUE(ServerResponse::CheckFilesystem(def_res_address_ + "/"));
    EXPECT_TRUE(ServerResponse::CheckFilesystem(def_res_address_ + "/loc_1"));
    EXPECT_TRUE(ServerResponse::CheckFilesystem(def_res_address_ + "/loc_1/loc_3/loc_3_in_loc_1_404.html"));
}

TEST_F(LocationSynthUtilsCheckFilesystem, CheckFilesystemNotExist) {
    EXPECT_FALSE(ServerResponse::CheckFilesystem(def_res_address_ + "/X"));
    EXPECT_FALSE(ServerResponse::CheckFilesystem(def_res_address_ + "/loc_1/X"));
    EXPECT_FALSE(ServerResponse::CheckFilesystem(def_res_address_ + "/loc_1/loc_4/X"));
}

// todo: more tests with ip addressess
static Config conf_get("test_resources/limit_except/nginx_get.conf");
class LocationSynthUtilsCheckLimitExceptGet : public ::testing::Test, public Server {
public:
    LocationSynthUtilsCheckLimitExceptGet() : Server(conf_get.getConstServers().front()) {};
};

TEST_F(LocationSynthUtilsCheckLimitExceptGet, GET) {
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    auto loc_1_1 = GetConfig().FindConstLocation("/loc_1/loc_1_1").location_;

    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    auto loc_2_2 = GetConfig().FindConstLocation("/loc_2/loc_2_2").location_;

    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    auto loc_3_3 = GetConfig().FindConstLocation("/loc_3/loc_3_3").location_;

    auto loc_4 = GetConfig().FindConstLocation("/loc_4").location_;
    auto loc_4_4 = GetConfig().FindConstLocation("/loc_4/loc_4_4").location_;

    EXPECT_FALSE(AccessForbidden(loc_1, GET));
    EXPECT_FALSE(AccessForbidden(loc_1_1, GET));
    EXPECT_FALSE(AccessForbidden(loc_2, GET));
    EXPECT_FALSE(AccessForbidden(loc_2_2, GET));
    EXPECT_FALSE(AccessForbidden(loc_3, GET));
    EXPECT_FALSE(AccessForbidden(loc_3_3, GET));
    EXPECT_FALSE(AccessForbidden(loc_4, GET));
    EXPECT_FALSE(AccessForbidden(loc_4_4, GET));

    EXPECT_TRUE(AccessForbidden(loc_1, POST));
    EXPECT_FALSE(AccessForbidden(loc_1_1, POST));

    EXPECT_TRUE(AccessForbidden(loc_2, POST));
    EXPECT_FALSE(AccessForbidden(loc_2_2, POST));

    EXPECT_FALSE(AccessForbidden(loc_3, POST));
    EXPECT_FALSE(AccessForbidden(loc_3_3, POST));

    EXPECT_TRUE(AccessForbidden(loc_4, POST));
    EXPECT_FALSE(AccessForbidden(loc_4_4, POST));
}

static Config conf_post("test_resources/limit_except/nginx_post.conf");
class LocationSynthUtilsCheckLimitExceptPost : public ::testing::Test, public
        Server {
public:
    LocationSynthUtilsCheckLimitExceptPost() : Server(conf_post.getConstServers().front()) {};
};

TEST_F(LocationSynthUtilsCheckLimitExceptPost, POST) {
    auto loc_1 = GetConfig().FindConstLocation("/loc_1").location_;
    auto loc_1_1 = GetConfig().FindConstLocation("/loc_1/loc_1_1").location_;

    auto loc_2 = GetConfig().FindConstLocation("/loc_2").location_;
    auto loc_2_2 = GetConfig().FindConstLocation("/loc_2/loc_2_2").location_;

    auto loc_3 = GetConfig().FindConstLocation("/loc_3").location_;
    auto loc_3_3 = GetConfig().FindConstLocation("/loc_3/loc_3_3").location_;

    auto loc_4 = GetConfig().FindConstLocation("/loc_4").location_;
    auto loc_4_4 = GetConfig().FindConstLocation("/loc_4/loc_4_4").location_;

    EXPECT_TRUE(AccessForbidden(loc_1, GET));
    EXPECT_FALSE(AccessForbidden(loc_1_1, GET));

    EXPECT_TRUE(AccessForbidden(loc_2, GET));
    EXPECT_FALSE(AccessForbidden(loc_2_2, GET));

    EXPECT_FALSE(AccessForbidden(loc_3, GET));
    EXPECT_FALSE(AccessForbidden(loc_3_3, GET));

    EXPECT_TRUE(AccessForbidden(loc_4, GET));
    EXPECT_FALSE(AccessForbidden(loc_4_4, GET));

    EXPECT_FALSE(AccessForbidden(loc_1, POST));
    EXPECT_FALSE(AccessForbidden(loc_1_1, POST));
    EXPECT_FALSE(AccessForbidden(loc_2, POST));
    EXPECT_FALSE(AccessForbidden(loc_2_2, POST));
    EXPECT_FALSE(AccessForbidden(loc_3, POST));
    EXPECT_FALSE(AccessForbidden(loc_3_3, POST));
    EXPECT_FALSE(AccessForbidden(loc_4, POST));
    EXPECT_FALSE(AccessForbidden(loc_4_4, POST));
}