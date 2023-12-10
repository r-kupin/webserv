#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"

class ReturnHandlerTest : public ::testing::Test, public Location {
public:
    explicit ReturnHandlerTest()
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

TEST_F(ReturnHandlerTest, RedirectCodeAndAddress) {
    Location redirect_sub("/redirect");
    redirect_sub.HandleLocationReturn(v_str({"return", "301",
                                             "http://localhost:4280/goto"}));
    EXPECT_EQ(redirect_sub.return_code_, 301);
    EXPECT_EQ(redirect_sub.return_address_, "http://localhost:4280/goto");
    EXPECT_EQ(redirect_sub.return_custom_message_, "");
}

TEST_F(ReturnHandlerTest, RedirectOnlyCode) {
    Location redirect_sub("/redirect");
    redirect_sub.HandleLocationReturn(v_str({"return", "301"}));
    EXPECT_EQ(redirect_sub.return_code_, 301);
    EXPECT_EQ(redirect_sub.return_address_, "");
    EXPECT_EQ(redirect_sub.return_custom_message_, "");
}

TEST_F(ReturnHandlerTest, RedirectOnlyAddress) {
    Location redirect_sub("/redirect");
    redirect_sub.HandleLocationReturn(v_str({"return",
                                             "http://localhost:4280/goto"}));
//  todo  EXPECT_EQ(redirect_sub.return_code_, ???? );
    EXPECT_EQ(redirect_sub.return_address_, "http://localhost:4280/goto");
    EXPECT_EQ(redirect_sub.return_custom_message_, "");
}

TEST_F(ReturnHandlerTest, NonRedirectOnlyCode) {
    Location forbidden("/forbidden");
    forbidden.HandleLocationReturn(v_str({"return", "403"}));
    EXPECT_EQ(forbidden.return_code_, 403);
    EXPECT_EQ(forbidden.return_custom_message_, "");
}

TEST_F(ReturnHandlerTest, NonRedirectCodeAndCustomMessage) {
    Location forbidden("/forbidden");
    forbidden.HandleLocationReturn(v_str({ "return", "403",
                                           "Blah" }));
    EXPECT_EQ(forbidden.return_code_, 403);
    EXPECT_EQ(forbidden.return_custom_message_, "Blah");
}

TEST_F(ReturnHandlerTest, WrongRedirectCustomMessage) {
    Location redirect_sub("/redirect");

    EXPECT_THROW(redirect_sub.HandleLocationReturn(
            v_str({"return", "301", "Blah"})), LocationException);
}

TEST_F(ReturnHandlerTest, WrongCustomMessage) {
    Location redirect_sub("/wrong");

    EXPECT_THROW(redirect_sub.HandleLocationReturn(
            v_str({"return", "Blah"})), LocationException);
}

TEST_F(ReturnHandlerTest, WrongMessageAndAddress) {
    Location forbidden("/wrong");
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "Blah", "http://localhost:4280/sub"})),
                 LocationException);
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "http://localhost:4280/sub", "Blah"})),
                 LocationException);
}

TEST_F(ReturnHandlerTest, WrongOrderOfCodeAndAddressOrMessage) {
    Location forbidden("/wrong");
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "http://localhost:4280/sub", "301"})),
                 LocationException);
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "Blah", "403"})),
                 LocationException);
}

TEST_F(ReturnHandlerTest, WrongTwoCodes) {
    Location forbidden("/wrong");
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "302", "301"})),
                 LocationException);
}

TEST_F(ReturnHandlerTest, WrongTwoAddresses) {
    Location forbidden("/wrong");
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "http://localhost:4280/sub", "http://localhost:4280/sub"})),
                 LocationException);
}

TEST_F(ReturnHandlerTest, WrongArgsAmount) {
    Location forbidden("/wrong");
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "301", "http://localhost:4280/sub", "Blah"})),
                 LocationException);
}