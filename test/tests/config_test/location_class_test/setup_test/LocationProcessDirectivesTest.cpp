#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Server/ServerExceptions.h"

class ProcessDerectivesTest : public ::testing::Test, public Location {
public:
    explicit ProcessDerectivesTest() : Location() {};
protected:
    ServerConfiguration sc;
    std::vector<v_str> directives_;
    std::vector<v_str> directives_2;
};

TEST_F(ProcessDerectivesTest, TestForNewLocation) {
    Location loc = Location("/loc", sc.GetRootIt());

    directives_.push_back({"root", "/nonexistent"});
    directives_.push_back({"index", "nonexistent_index.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error.html"});
    directives_.push_back({"return", "301", "http://localhost:4280/somewhere"});

    EXPECT_NO_THROW(loc.ProcessDirectives(directives_));

    EXPECT_EQ(loc.root_, "/nonexistent");

    EXPECT_TRUE(loc.index_defined_);
    EXPECT_EQ(loc.index_.size(), 1);
    EXPECT_EQ(*loc.index_.begin(), "nonexistent_index.html");

    EXPECT_EQ(loc.return_code_, 301);
    EXPECT_EQ(loc.return_address_, "http://localhost:4280/somewhere");

    EXPECT_NE(loc.error_pages_.find(ErrPage("error.html", 403)),
              loc.error_pages_.end());
    EXPECT_NE(loc.error_pages_.find(ErrPage("error.html", 400)),
              loc.error_pages_.end());
    EXPECT_NE(loc.error_pages_.find(ErrPage("error.html", 416)),
              loc.error_pages_.end());
}

TEST_F(ProcessDerectivesTest, TestForRootRootRedefinition) {
//-----------------------init directives set------------------------------------
    directives_.push_back({"root", "/root1"});
    directives_.push_back({"index", "index_1.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error1.html"});

    directives_2.push_back({"root", "/root2"});
    directives_2.push_back({"index", "index_2.html"});
    directives_2.push_back({"error_page", "403", "400", "416", "error2.html"});

//-----------------------define root--------------------------------------------
    Location &root_loc = sc.GetRoot();
    EXPECT_NO_THROW(root_loc.ProcessDirectives(directives_));

    EXPECT_EQ(root_loc.root_, "/root1");
    EXPECT_NE(std::find(root_loc.index_.begin(), root_loc.index_.end(),"index_1.html"), root_loc.index_.end());

    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error1.html", 403)),
              root_loc.error_pages_.end());
    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error1.html", 400)),
              root_loc.error_pages_.end());
    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error1.html", 416)),
              root_loc.error_pages_.end());

//---------------------redefine root--------------------------------------------
    Location &root_loc2 = sc.GetRoot();
    EXPECT_NO_THROW(root_loc.ProcessDirectives(directives_2));

    EXPECT_EQ(sc.locations_.size(), 1);
    EXPECT_EQ(root_loc2.sublocations_.size(), 0);

    EXPECT_EQ(root_loc2.root_, "/root2");
    EXPECT_NE(std::find(root_loc2.index_.begin(), root_loc2.index_.end(),"index_2.html"), root_loc2.index_.end());

    EXPECT_NE(root_loc2.error_pages_.find(ErrPage("error2.html", 403)),
              root_loc2.error_pages_.end());
    EXPECT_NE(root_loc2.error_pages_.find(ErrPage("error2.html", 400)),
              root_loc2.error_pages_.end());
    EXPECT_NE(root_loc2.error_pages_.find(ErrPage("error2.html", 416)),
              root_loc2.error_pages_.end());
}