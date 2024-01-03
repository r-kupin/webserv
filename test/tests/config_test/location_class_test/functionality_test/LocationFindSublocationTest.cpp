#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"

class FindSublocationTest : public ::testing::Test, public ServerConfiguration {
public:
    explicit FindSublocationTest() : ServerConfiguration(), root_(GetRootIt()) {
        root_->sublocations_.push_front(Location("/sub1"));
        sub1_ = root_->sublocations_.begin();
        sub1_->own_index_.push_back("sub1_index.html");
        sub1_->parent_ = root_;

        root_->sublocations_.push_front(Location("/sub2"));
        sub2_ = root_->sublocations_.begin();
        sub2_->own_index_.push_back("sub2_index.html");
        sub2_->parent_ = root_;

        sub2_->sublocations_.push_front(Location("/sub2/sub3"));
        sub3_ = sub2_->sublocations_.begin();
        sub3_->own_index_.push_back("sub3_index.html");
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
    auto found = root_->FindConstSublocationByAddress("/sub1");

    EXPECT_EQ(*found, *sub1_);
    EXPECT_EQ(*found->parent_, *root_);
}

TEST_F(FindSublocationTest, ExistingFindAnotherSublocationByAddressTest) {
    auto found = root_->FindConstSublocationByAddress("/sub2");

    EXPECT_EQ(*found, *sub2_);
    EXPECT_EQ(*found->parent_, *root_);
}

TEST_F(FindSublocationTest, ExistingFindSubSublocationByAddressTest) {
    auto found = sub2_->FindConstSublocationByAddress("/sub3");

    EXPECT_EQ(*found, *sub3_);
    EXPECT_EQ(*found->parent_, *sub2_);
    EXPECT_EQ(*found->parent_->parent_, *root_);
    EXPECT_EQ(*found->parent_->parent_->parent_->parent_, *root_);
}

class GetRootTest : public FindSublocationTest {};

TEST_F(GetRootTest, AllRootsAreRoot) {
    auto found_1 = root_->FindConstSublocationByAddress("/sub1");
    auto found_2 = root_->FindConstSublocationByAddress("/sub2");
    auto found_3 = sub2_->FindConstSublocationByAddress("/sub3");

    auto from_conf_3 = FindConstLocation("/sub3").location_;
    auto from_conf_2 = FindConstLocation("/sub2").location_;
    auto from_conf_1 = FindConstLocation("/sub1").location_;
    auto from_conf = FindConstLocation("/").location_;

    EXPECT_EQ(root_->GetMyRootRef(), *root_);
    EXPECT_EQ(found_1->GetMyRootRef(), *root_);
    EXPECT_EQ(found_1->GetMyRootRef(), found_2->GetMyRootRef());
    EXPECT_EQ(found_1->GetMyRootRef(), found_3->GetMyRootRef());

    EXPECT_EQ(from_conf->GetMyRootRef(), *root_);
    EXPECT_EQ(from_conf_1->GetMyRootRef(), *root_);
    EXPECT_EQ(from_conf_1->GetMyRootRef(), from_conf_2->GetMyRootRef());
    EXPECT_EQ(from_conf_1->GetMyRootRef(), from_conf_3->GetMyRootRef());

    EXPECT_EQ(root_->GetMyRootIt(), root_);
    EXPECT_EQ(found_1->GetMyRootIt(), root_);
    EXPECT_EQ(found_1->GetMyRootIt(), found_2->GetMyRootIt());
    EXPECT_EQ(found_1->GetMyRootIt(), found_3->GetMyRootIt());

    EXPECT_EQ(from_conf->GetMyRootIt(), root_);
    EXPECT_EQ(from_conf_1->GetMyRootIt(), root_);
    EXPECT_EQ(from_conf_1->GetMyRootIt(), from_conf_2->GetMyRootIt());
    EXPECT_EQ(from_conf_1->GetMyRootIt(), from_conf_3->GetMyRootIt());
}