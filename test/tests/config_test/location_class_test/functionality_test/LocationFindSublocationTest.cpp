#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Server/ServerExceptions.h"

class FindSublocationTest : public ::testing::Test, public ServerConfiguration {
public:
    explicit FindSublocationTest() : ServerConfiguration() {
        l_loc_it root = locations_.begin();

        root->sublocations_.push_back(Location("/sub1"));
        l_loc_it sub1 = root->sublocations_.begin();
        sub1->index_.push_back("sub_index.html");
        sub1->parent_ = root;

        root->sublocations_.push_back(Location("/sub2"));
        l_loc_it sub2 = root->sublocations_.begin().;
        sub2->index_.push_back("sub_index.html");
        sub2->parent_ = locations_.begin();
        sub2->sublocations_.push_back(Location("/sub3"));
        Location & sub3 = root.sublocations_.front();
        sub2.index_.push_back("sub_index.html");
        sub2.parent_ = locations_.begin();
    };
protected:
    std::list<Location> imaginary_config_;
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