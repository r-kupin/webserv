#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Server/ServerExceptions.h"

class FindSublocationTest : public ::testing::Test, public ServerConfiguration {
public:
    explicit FindSublocationTest() : ServerConfiguration(), root_(GetRootIt()) {
        root_->sublocations_.push_front(Location("/sub1"));
        sub1_ = root_->sublocations_.begin();
        sub1_->index_.push_back("sub1_index.html");
        sub1_->parent_ = root_;

        root_->sublocations_.push_front(Location("/sub2"));
        sub2_ = root_->sublocations_.begin();
        sub2_->index_.push_back("sub2_index.html");
        sub2_->parent_ = root_;

        sub2_->sublocations_.push_front(Location("/sub3"));
        sub3_ = sub2_->sublocations_.begin();
        sub3_->index_.push_back("sub3_index.html");
        sub3_->parent_ = sub2_;
    };
protected:
    l_loc_it root_;
    l_loc_it sub1_;
    l_loc_it sub2_;
    l_loc_it sub3_;
};

TEST_F(FindSublocationTest, RootFindSublocationByAddressTest) {
    EXPECT_EQ(*root_->FindConstSublocationByAddress("/"), *root_);
}

TEST_F(FindSublocationTest, ExistingFindSublocationByAddressTest) {
    const l_loc_c_it &found = root_->FindConstSublocationByAddress("/sub1");

    EXPECT_EQ(*found, *sub1_);
    EXPECT_EQ(*found->parent_, *root_);
}

TEST_F(FindSublocationTest, ExistingFindAnotherSublocationByAddressTest) {
    const l_loc_c_it &found = root_->FindConstSublocationByAddress("/sub2");

    EXPECT_EQ(*found, *sub2_);
    EXPECT_EQ(*found->parent_, *root_);
}

TEST_F(FindSublocationTest, ExistingFindSubSublocationByAddressTest) {
    const l_loc_c_it &found = sub2_->FindConstSublocationByAddress("/sub3");

    EXPECT_EQ(*found, *sub3_);
    EXPECT_EQ(*found->parent_, *sub2_);
    EXPECT_EQ(*found->parent_->parent_, *root_);
    EXPECT_EQ(*found->parent_->parent_->parent_->parent_, *root_);
}
