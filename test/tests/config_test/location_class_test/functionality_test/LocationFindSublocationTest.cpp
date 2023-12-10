#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Server/ServerExceptions.h"

class FindSublocationTest : public ::testing::Test, public Location {
public:
    explicit FindSublocationTest()
    : Location(),
      sc_root_(sc_.GetRoot()) {
        sc_root_.sublocations_.push_back(Location("/sub"));
        sc_root_.sublocations_.begin()->index_.push_back("sub_index.html");
        sc_root_.sublocations_.begin()->parent_ = sc_.GetRootIt();
    };
protected:
    ServerConfiguration sc_;
    Location & sc_root_;
};

TEST_F(FindSublocationTest, NonExistingFindSublocationByAddressTest) {
    EXPECT_THROW(sc_root_.FindSublocationByAddress(""), NotFoundException);
}

TEST_F(FindSublocationTest, RootFindSublocationByAddressTest) {
    const Location & loc = *sc_root_.FindSublocationByAddress("/");

    EXPECT_EQ(loc.address_, "/");
}

TEST_F(FindSublocationTest, ExistingFindSublocationByAddressTest) {
    const Location & loc = *sc_root_.FindSublocationByAddress("/sub");

    EXPECT_EQ(loc.address_, "/sub");
    EXPECT_EQ(*loc.index_.begin(), "sub_index.html");
    EXPECT_EQ(*loc.parent_, sc_root_);
    EXPECT_EQ(*loc.parent_, *sc_.GetRootIt());
}

TEST_F(FindSublocationTest, HasSameAddressAsOneOfSublocationsOf) {
    Location loc1("/loc1");
    loc1.index_.push_back("loc1_index.html");
    sc_root_.sublocations_.push_back(loc1);

    Location loc2("/loc2");
    loc2.index_.push_back("loc2_index.html");

    EXPECT_TRUE(loc1.HasSameAddressAsOneOfSublocationsOf(sc_root_));
    EXPECT_FALSE(loc2.HasSameAddressAsOneOfSublocationsOf(sc_root_));
}