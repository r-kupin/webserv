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

class LocationSynthUtilsCheckLimitExcept : public ::testing::Test, public Server {
public:
    LocationSynthUtilsCheckLimitExcept()
    : Server(Config("test_resources/nested_locations/nginx.conf").getConstServers().front()) {};
};
//
//TEST_F(LocationSynthUtilsCheckLimitExcept, CheckLimitExcept) {
//
//
//    EXPECT_TRUE(CheckLimitedAccess(found, Methods::GET));
//    EXPECT_FALSE(CheckLimitedAccess(found, Methods::POST));
//
//    EXPECT_TRUE(CheckLimitedAccess(*found.parent_, Methods::GET));
//    EXPECT_FALSE(CheckLimitedAccess(*found.parent_, Methods::POST));
//}