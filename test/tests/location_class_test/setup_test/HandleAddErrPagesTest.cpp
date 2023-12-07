#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../src/Config/Config.h"

class ErrorPagesAddingTest : public ::testing::Test, public Location {
public:
    explicit ErrorPagesAddingTest()
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

TEST_F(ErrorPagesAddingTest, AddErrPages) {
    sc_root_.AddErrorPages(v_str({"error_page", "404", "/404.html"}));
    sc_root_.AddErrorPages(v_str({"error_page", "403", "/403.html"}));

    EXPECT_EQ(sc_root_.error_pages_.find(
            ErrPage("/404.html", 404))->address_, "/404.html");
    EXPECT_EQ(sc_root_.error_pages_.find(
            ErrPage("/404.html", 400)), sc_root_.error_pages_.end());
}

TEST_F(ErrorPagesAddingTest, UpdateErrPages) {
    sc_root_.AddErrorPages(v_str({"error_page", "404", "/404.html"}));

    EXPECT_EQ(sc_root_.error_pages_.find(
            ErrPage("/404.html", 404))->address_, "/404.html");

    sc_root_.AddErrorPages(v_str({"error_page", "404", "/404_new.html"}));

    EXPECT_EQ(sc_root_.error_pages_.find(
            ErrPage("/404_new.html", 404))->address_, "/404_new.html");
}

TEST_F(ErrorPagesAddingTest, ErrPagesWrongDirective) {
    EXPECT_THROW(sc_root_.AddErrorPages(v_str({"error_page", "404"})),
                 LocationException);
    EXPECT_THROW(sc_root_.AddErrorPages(v_str({"error_page", "304"})),
                 LocationException);
    EXPECT_THROW(sc_root_.AddErrorPages(v_str({"error_page", "404", "/aaa", "bbb"})),
                 LocationException);
    EXPECT_THROW(sc_root_.AddErrorPages(v_str({"error_page", "zzz", "/aaa"})),
                 LocationException);
}

