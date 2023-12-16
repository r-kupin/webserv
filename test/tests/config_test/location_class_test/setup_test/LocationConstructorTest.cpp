#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"


class LocationConstructorTest : public ::testing::Test, public Location {

};

TEST_F(LocationConstructorTest, HandleAddressInConstructorThrowsTest) {
    EXPECT_THROW(HandleAddressInConstructor("home"), LocationException);
    EXPECT_THROW(HandleAddressInConstructor("home/"), LocationException);
    EXPECT_THROW(HandleAddressInConstructor("home/home"), LocationException);
    EXPECT_THROW(HandleAddressInConstructor("/ho?me"), LocationException);
    EXPECT_THROW(HandleAddressInConstructor("/ho?me"), LocationException);
    EXPECT_THROW(HandleAddressInConstructor("/home/w?tf"), LocationException);
    EXPECT_THROW(HandleAddressInConstructor("/ho?me/w?tf"), LocationException);
}

TEST_F(LocationConstructorTest, HandleAddressInConstructorCorrectsTest) {
    EXPECT_EQ(HandleAddressInConstructor("/home"), "/home");
    EXPECT_EQ(HandleAddressInConstructor("/home/"), "/home");
    EXPECT_EQ(HandleAddressInConstructor("/home/home"), "/home/home");
    EXPECT_EQ(HandleAddressInConstructor("/home/home/"), "/home/home");
    EXPECT_EQ(HandleAddressInConstructor("//home///home//"), "/home/home");
}

TEST_F(LocationConstructorTest, GetParticularAddressConstructorCorrectsTest) {
    EXPECT_EQ(GetParticularAddress("/loc"), "/loc");
    EXPECT_EQ(GetParticularAddress("/loc1/loc"), "/loc");
}
