/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSyntFoundExact.cpp                         :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/14 17:39:52 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

class LocationSyntFoundExact : public ::testing::Test, public Server {
public:
    explicit LocationSyntFoundExact()
    : Server(Config("test_resources/nested_locations/nginx.conf").getConstServers().front()) {};
protected:
    ClientRequest cl_req;
};



//
//
//TEST_F(LocationSynthTest,
//        location_not_defined_directory_does_not_exists) {
//    std::string uri = "/loc_X";
//
//    LocSearchResult res = FindConstLocation(uri);
//    EXPECT_EQ(res.location_, GetConfig().GetRoot());
//    EXPECT_EQ(res.status_, "not found");
//}
//
//TEST_F(LocationSynthTest,
//       location_defined_directory_does_not_exists_index_file_not_defined) {
//    std::string uri = "/loc_1X";
//
//    LocSearchResult res = FindConstLocation(uri);
//    EXPECT_EQ(res.location_, GetConfig().GetRoot());
//    EXPECT_EQ(res.status_, "not found");
//}
//
//TEST_F(LocationSynthTest,
//       location_defined_directory_does_not_exists_index_file_defined) {
//    std::string uri = "/loc_2X";
//
//    LocSearchResult res = FindConstLocation(uri);
//    EXPECT_EQ(res.location_, GetConfig().GetRoot());
//    EXPECT_EQ(res.status_, "not found");
//}
//
//TEST_F(LocationSyntFoundExact, CheckFoundLocationPathDoesntExist) {
//    std::string status;
//    const Location &found = RecursiveSearch("/loc_defined_index_which_exist",
//                                            GetConfig().GetRoot(),
//                                            status);
//    // root = example/htmls/loc_defined_index_which_exist
//    // path = test_resources/test1/example/htmls/loc_defined_index_which_exist/loc_defined_index_which_exist
//    EXPECT_TRUE(CheckFilesystem(found.root_, "test_resources/test1/"));
//    EXPECT_FALSE(CheckFilesystem(found.root_, "test_resources/test2/"));
//}
//
//TEST_F(LocationSyntFoundExact, CheckFoundLocationAccessLimitation) {
//    std::string status;
//    const Location &found = RecursiveSearch("/uploads/something/whatever",
//                                            GetConfig().GetRoot(),
//                                            status);
//    EXPECT_TRUE(CheckLimitedAccess(found, Methods::GET));
//    EXPECT_FALSE(CheckLimitedAccess(found, Methods::POST));
//
//    EXPECT_TRUE(CheckLimitedAccess(*found.parent_, Methods::GET));
//    EXPECT_FALSE(CheckLimitedAccess(*found.parent_, Methods::POST));
//}
//
//TEST_F(LocationSyntFoundExact,
//       SynthesiseFor_ExactMatch_DirectoryExist_IndexDefinedButDontExist) {
//    std::string loc = "/loc_defined_index_not_exist";
//    std::string status;
//    const Location &found = RecursiveSearch(loc,
//                                            GetConfig().GetRoot(),
//                                            status);
//
//    std::string request = "GET " + loc + " HTTP/1.1\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//
//    Location synth(found);
//    synth = SynthFoundExact(cl_req, found, synth,
//                            "test_resources/test1/");
//    EXPECT_EQ(synth.return_code_, 403);
//}
//
//TEST_F(LocationSyntFoundExact,
//       SynthesiseFor_ExactMatch_DirectoryExist_IndexDefined_AndExist) {
//    std::string loc = "/loc_defined_index_which_exist";
//    std::string status;
//    const Location &found = RecursiveSearch(loc,
//                                            GetConfig().GetRoot(),
//                                            status);
//
//    std::string request = "GET " + loc + " HTTP/1.1\n\r";
//
//    pipe_reguest_to_fd(request);
//    ClientRequest cl_req(fd_);
//
//    Location synth(found);
//    synth = SynthFoundExact(cl_req, found, synth,
//                            "test_resources/test1/");
//    EXPECT_EQ(synth.return_code_, 200);
//}
