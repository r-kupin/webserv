#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

class SimpleConfigLocationSearchTest : public ::testing::Test, public Server {
public:
    explicit SimpleConfigLocationSearchTest()
    : Server(Config("test_resources/simple/nginx.conf").
                                                        getServers().front()){}
protected:
    std::string uri_;
};

TEST_F(SimpleConfigLocationSearchTest, FindRootLocation) {
    uri_ = "/";
    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_, getConfig().GetRoot());
    EXPECT_EQ(result.status_, "found");
}

TEST_F(SimpleConfigLocationSearchTest, FindDefinedLocation) {
    uri_ = "/loc_1";
    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_.address_, uri_);
    EXPECT_EQ(result.location_.parent_->address_, "/");
    EXPECT_EQ(result.status_, "found");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedRootSubLocation) {
    uri_ = "/loc_XXX";
    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_, getConfig().GetRoot());
    EXPECT_EQ(result.status_, "not found");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedSubLocationOfNonRoot) {
    uri_ = "loc_1/loc_XXX";
    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_.address_, "/loc_1");
    EXPECT_EQ(result.location_.parent_->address_, "/");
    EXPECT_EQ(result.status_, "not found");
}
