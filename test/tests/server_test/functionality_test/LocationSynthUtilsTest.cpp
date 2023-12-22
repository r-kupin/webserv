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

class LocationSynthUtilsCheckLimitExcept : public ::testing::Test, public Server {
public:
    LocationSynthUtilsCheckLimitExcept()
    : Server(), conf_(const_cast<ServerConfiguration&>(GetConfig())) {};
protected:
    ServerConfiguration &conf_;
    
    virtual void SetUp() {
        conf_.HandleLocationContext(
                Node(
                        v_str({"location", "/loc_2"}),
                        std::vector<Node>({
                            Node(v_str({"limit_except", "POST"}),
                                 std::vector<v_str>({
                                     v_str({"allow", "all"})}))
                        })
                )
        );
        conf_.HandleLocationContext(
                Node(
                        v_str({"location", "/loc_2/subloc_2"}),
                        std::vector<v_str>({
                            v_str({ "index", "index.html" })
                        })
                )
        );

        conf_.HandleLocationContext(
                Node(
                        v_str({"location", "/loc_3"}),
                        std::vector<v_str>({
                            v_str({ "index", "index.html" })
                        })
                )
        );
        conf_.HandleLocationContext(
                Node(
                        v_str({"location", "/loc_3/subloc_3"}),
                        std::vector<v_str>({
                            v_str({ "index", "index.html" })
                        })
                )
        );

    }
};

TEST_F(LocationSynthUtilsCheckLimitExcept, CheckDenyAll) {
    conf_.HandleLocationContext(
            Node(
                    v_str({"location", "/loc_1"}),
                    std::vector<v_str>(),
                    std::vector<Node>({
                        Node(v_str({"limit_except", "POST"}),
                             std::vector<v_str>({
                                 v_str({"deny", "all"})}))
                    })
            )
    );
    conf_.HandleLocationContext(
            Node(
                    v_str({"location", "/loc_1/subloc_1"}),
                    std::vector<v_str>({
                        v_str({ "index", "index.html" })
                    })
            )
    );

    auto deny_all_location = conf_.FindLocation("/loc_1").location_;
    auto deny_all_sublocation = conf_.FindLocation("/loc_1/subloc_1").location_;

    EXPECT_FALSE(CheckLimitedAccess(*deny_all_location, Methods::POST));
    EXPECT_FALSE(CheckLimitedAccess(*deny_all_sublocation, Methods::POST));

    EXPECT_TRUE(CheckLimitedAccess(*deny_all_location, Methods::GET));
    EXPECT_TRUE(CheckLimitedAccess(*deny_all_sublocation, Methods::GET));
}