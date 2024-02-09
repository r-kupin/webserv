#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"


class LocationStaticToolsTest : public ::testing::Test {
};

TEST_F(LocationStaticToolsTest, HandleAddressInConstructorThrowsTest) {
    EXPECT_THROW(Location::HandleAddressInConstructor("home"),
                 Location::LocationException);
    EXPECT_THROW(Location::HandleAddressInConstructor("home/"),
                 Location::LocationException);
    EXPECT_THROW(Location::HandleAddressInConstructor("home/home"),
                 Location::LocationException);
    EXPECT_THROW(Location::HandleAddressInConstructor("/ho?me"),
                 Location::LocationException);
    EXPECT_THROW(Location::HandleAddressInConstructor("/ho?me"),
                 Location::LocationException);
    EXPECT_THROW(Location::HandleAddressInConstructor("/home/w?tf"),
                 Location::LocationException);
    EXPECT_THROW(Location::HandleAddressInConstructor("/ho?me/w?tf"),
                 Location::LocationException);
}

TEST_F(LocationStaticToolsTest, HandleAddressInConstructorCorrectsTest) {
    EXPECT_EQ(Location::HandleAddressInConstructor("/home"), "/home");
    EXPECT_EQ(Location::HandleAddressInConstructor("/home/"), "/home");
    EXPECT_EQ(Location::HandleAddressInConstructor("/home/home"), "/home/home");
    EXPECT_EQ(Location::HandleAddressInConstructor("/home/home/"),
              "/home/home");
    EXPECT_EQ(Location::HandleAddressInConstructor("//home///home//"),
              "/home/home");
}

TEST_F(LocationStaticToolsTest, GetParticularAddressConstructorCorrectsTest) {
    EXPECT_EQ(Location::GetParticularAddress("/loc"), "/loc");
    EXPECT_EQ(Location::GetParticularAddress("/loc1/loc"), "/loc");
}

TEST_F(LocationStaticToolsTest, SupressConsecutiveSlashesTest) {
    EXPECT_EQ(Location::SupressConsecutiveSlashes("/loc"), "/loc");
    EXPECT_EQ(Location::SupressConsecutiveSlashes("/loc/"), "/loc");
    EXPECT_EQ(Location::SupressConsecutiveSlashes("//////loc/"), "/loc");
    EXPECT_EQ(Location::SupressConsecutiveSlashes("/loc/////////"), "/loc");
    EXPECT_EQ(Location::SupressConsecutiveSlashes("/loc/////loc"), "/loc/loc");
    EXPECT_EQ(Location::SupressConsecutiveSlashes("//////////"), "/");
}

TEST_F(LocationStaticToolsTest, SplitAddressTest) {
    EXPECT_EQ(Location::SplitAddress("/"), v_str({"/"}));
    EXPECT_EQ(Location::SplitAddress("/loc"), v_str({"/loc"}));
    EXPECT_EQ(Location::SplitAddress("/loc1/loc2/loc3"),
                                            v_str({"/loc1", "/loc2", "/loc3"}));
    EXPECT_EQ(Location::SplitAddress("/loc1/loc2/loc3/"),
                                            v_str({"/loc1", "/loc2", "/loc3"}));
}
