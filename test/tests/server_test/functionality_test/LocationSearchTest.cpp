#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"
#include "../../../../src/Server/ServerExceptions.h"

class RequestHandlingTest : public ::testing::Test, public Server {
public:
    explicit RequestHandlingTest() : Server(
            *Config("test_resources/simple/nginx.conf").getServers().begin()) {}
};

TEST_F(RequestHandlingTest, FindRootLocation) {
    std::string uri = "/";
    const LocSearchResult &result = FindLocation(uri);
    EXPECT_EQ(result.location_, getConfig().GetRoot());
    EXPECT_EQ(result.status_, "found");
}
// to synthesizer! not here!
//TEST_F(RequestHandlingTest,
//        location_not_defined_directory_does_not_exists) {
//    std::string uri = "/loc_X";
//
//    LocSearchResult res = FindLocation(uri);
//    EXPECT_EQ(res.location_, getConfig().GetRoot());
//    EXPECT_EQ(res.status_, "not found");
//}
//
//TEST_F(RequestHandlingTest,
//       location_defined_directory_does_not_exists_index_file_not_defined) {
//    std::string uri = "/loc_1X";
//
//    LocSearchResult res = FindLocation(uri);
//    EXPECT_EQ(res.location_, getConfig().GetRoot());
//    EXPECT_EQ(res.status_, "not found");
//}
//
//TEST_F(RequestHandlingTest,
//       location_defined_directory_does_not_exists_index_file_defined) {
//    std::string uri = "/loc_2X";
//
//    LocSearchResult res = FindLocation(uri);
//    EXPECT_EQ(res.location_, getConfig().GetRoot());
//    EXPECT_EQ(res.status_, "not found");
//}
