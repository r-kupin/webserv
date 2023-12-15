#include <gtest/gtest.h>
#include "../../../../src/Server/Server.h"

class SimpleConfigLocationSearchTest : public ::testing::Test, public Server {
public:
    SimpleConfigLocationSearchTest() : Server(
            Config("test_resources/simple/nginx.conf").getServers().front()) {};

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
    uri_ = "/XXX";
    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(*result.location_, getConfig().GetConstRoot());
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/XXX");
}

TEST_F(SimpleConfigLocationSearchTest, FindUnDefinedSubLocationOfNonRoot) {
    uri_ = "/loc_1/XXX";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, "/loc_1");
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/XXX");
}

class NestedConfigLocationSearchTest : public ::testing::Test, public Server {
public:
    NestedConfigLocationSearchTest(): Server(
            Config("test_resources/nested_locations/nginx.conf").getServers().front()) {};

protected:
    std::string uri_;
};

TEST_F(NestedConfigLocationSearchTest, FindRootLocation) {
    uri_ = "/";
    const LocSearchResult &result = FindLocation(uri_);

    EXPECT_EQ(*result.location_, getConfig().GetConstRoot());
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_uri_, "");
}

TEST_F(NestedConfigLocationSearchTest, FindDefinedLocation) {
    uri_ = "/loc_1";

    const LocSearchResult &result = FindLocation(uri_);
    std::cout << getConfig() << std::endl;
    EXPECT_EQ(result.location_->address_, uri_);
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_uri_, "");
}

TEST_F(NestedConfigLocationSearchTest, FindUnDefinedRootSubLocation) {
    uri_ = "/XXX";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(*result.location_, getConfig().GetConstRoot());
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/XXX");
}

TEST_F(NestedConfigLocationSearchTest, FindUnDefinedSubLocationOfNonRoot) {
    uri_ = "/loc_1/XXX";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, "/loc_1");
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/XXX");
}

TEST_F(NestedConfigLocationSearchTest, DefinedLocationInTwiceNestedRoot) {
    uri_ = "/loc_0X";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, uri_);
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_uri_, "");
}

TEST_F(NestedConfigLocationSearchTest, LocationIsDefinedButNotInTheRoot) {
    uri_ = "/loc_5";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, uri_);
    EXPECT_EQ(result.location_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/loc_5");
}

TEST_F(NestedConfigLocationSearchTest, SuperNested) {
    uri_ = "/loc_1/loc_5/loc_6/loc_8";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, "/loc_8");
    EXPECT_EQ(result.location_->parent_->address_, "/loc_6");
    EXPECT_EQ(result.location_->parent_->parent_->address_, "/loc_5");
    EXPECT_EQ(result.location_->parent_->parent_->parent_->address_, "/loc_1");
    EXPECT_EQ(result.location_->parent_->parent_->parent_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "found");
    EXPECT_EQ(result.leftower_uri_, "");
}

TEST_F(NestedConfigLocationSearchTest, Loc_6_SomeFile) {
    uri_ = "/loc_1/loc_5/loc_6/image.jpeg";

    const LocSearchResult &result = FindLocation(uri_);
    EXPECT_EQ(result.location_->address_, "/loc_6");
    EXPECT_EQ(result.location_->parent_->address_, "/loc_5");
    EXPECT_EQ(result.location_->parent_->parent_->address_, "/loc_1");
    EXPECT_EQ(result.location_->parent_->parent_->parent_->address_, "/");
    EXPECT_EQ(result.status_, "not found");
    EXPECT_EQ(result.leftower_uri_, "/image.jpeg");
}

