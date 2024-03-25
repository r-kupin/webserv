/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    SynthFileTest.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/14 17:39:52 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../ConfigFiles.h"

class SynthFileTest_SimpleIndexCheck : public ::testing::Test, public Server {
public:
    explicit SynthFileTest_SimpleIndexCheck()
            : Server(simple_index_check.getConstServers().front()) {};
};

TEST_F(SynthFileTest_SimpleIndexCheck, Found) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_4/index.html");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthFile(synth, res, COMM_FILE, "/loc_4/index.html");

    EXPECT_EQ(synth.return_code_, 200);
    EXPECT_EQ(synth.body_file_,
              "test_resources/index_order/www/loc_4/index.html");
}

TEST_F(SynthFileTest_SimpleIndexCheck, NotFound) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_X");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthFile(synth, res, NOTHING, "/loc_X");

    EXPECT_EQ(synth.return_code_, 404);
    EXPECT_TRUE(synth.body_file_.empty());
}

TEST_F(SynthFileTest_SimpleIndexCheck, Redirect) {
    Srch_c_Res res = GetConfig().FindConstLocation("/loc_4");
    l_loc_c_it found = res.location_;
    Location synth(*found);
    SynthFile(synth, res, DIRECTORY, "/loc_4");

    EXPECT_EQ(synth.return_code_, 301);
    EXPECT_EQ(synth.return_internal_address_, "/loc_4/");
}
