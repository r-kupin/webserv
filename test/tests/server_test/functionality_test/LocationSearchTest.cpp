#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

class SimpleConfigLocationSearchTest : public ::testing::Test, public Server {
public:
    SimpleConfigLocationSearchTest()
    : Server() {
        Config simple_config("test_resources/simple/nginx.conf");
        SetConfig(simple_config.getServers().front());
    };

protected:
    std::string uri_;
};

TEST_F(SimpleConfigLocationSearchTest, FindRootLocation) {
    uri_ = "/";
    const LocSearchResult &result = FindLocation(uri_);

    EXPECT_EQ(*result.location_, getConfig().GetConstRoot());
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_uri_, "");
}

TEST_F(SimpleConfigLocationSearchTest, FindDefinedLocation) {
    uri_ = "/loc_1";

    const LocSearchResult &result = FindLocation(uri_);
    std::cout << getConfig() << std::endl;
    EXPECT_EQ(result.location_->address_, uri_);
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_uri_, "");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedRootSubLocation) {
    uri_ = "/loc_XXX";
    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(*result.location_, getConfig().GetConstRoot());
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/loc_XXX");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedSubLocationOfNonRoot) {
    uri_ = "/loc_1/loc_XXX";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, "/loc_1");
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/loc_XXX");
}

