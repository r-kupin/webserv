#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../src/Config/config/Config.h"

TEST(LocationTest, LocationWithQuestionInName) {
    ServerConfiguration sc;
    EXPECT_THROW(Location("/ho?me"), Location::LocationException);
    EXPECT_THROW(Location("/ho?me", sc.GetRootIt()),Location::LocationException);
    EXPECT_THROW(Location("/home/w?tf"), Location::LocationException);
    EXPECT_THROW(Location("/ho?me/w?tf", sc.GetRootIt()),Location::LocationException);
}
