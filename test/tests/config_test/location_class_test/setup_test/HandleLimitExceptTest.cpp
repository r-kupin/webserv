#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/server_configuration/ServerConfiguration.h"

class HandleLimitExceptTest  : public ::testing::Test, public Location {
public:
    explicit HandleLimitExceptTest() : Location(),
                                       sc_root_(sc_.GetRoot()) {
        sc_root_.sublocations_.push_back(Location("/sub"));
        sc_root_.sublocations_.begin()->index_.push_back("sub_index.html");
        sc_root_.sublocations_.begin()->parent_ = sc_.GetRootIt();
    };
protected:
    ServerConfiguration sc_;
    Location            &sc_root_;
    Node context_;
};

TEST_F(HandleLimitExceptTest, LimitExceptCorrectHTTPMethods) {
    context_.main_ = v_str({"limit_except", "GET" });
    context_.directives_.push_back(v_str({"deny", "all"}));

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(context_));
    EXPECT_TRUE(sc_root_.limit_except_.deny_all_);
    EXPECT_FALSE(sc_root_.limit_except_.allow_all_);
}

TEST_F(HandleLimitExceptTest, LimitExceptAllCorrectHTTPMethods) {
    context_.main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
    context_.directives_.push_back(v_str({"allow", "all"}));

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(context_));
    EXPECT_FALSE(sc_root_.limit_except_.deny_all_);
    EXPECT_TRUE(sc_root_.limit_except_.allow_all_);
}

TEST_F(HandleLimitExceptTest, TwoDirectives) {
    context_.main_ = v_str({"limit_except", "GET" });
    context_.directives_.push_back({"deny", "all"});
    context_.directives_.push_back({"allow", "172.17.0.1"});

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(context_));
    EXPECT_EQ(sc_root_.limit_except_.deny_all_, true);
    EXPECT_EQ(sc_root_.limit_except_.allow_all_, false);
    EXPECT_EQ(sc_root_.limit_except_.deny_.size(), 0);
    EXPECT_EQ(sc_root_.limit_except_.allow_.size(), 1);
    EXPECT_EQ(sc_root_.limit_except_.allow_[0], "172.17.0.1");
}

TEST_F(HandleLimitExceptTest, TwoDirectives2) {
    context_.main_ = v_str({"limit_except", "GET" });
    context_.directives_.push_back({"allow", "all"});
    context_.directives_.push_back({"deny", "172.17.0.1"});

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(context_));
    EXPECT_EQ(sc_root_.limit_except_.deny_all_, false);
    EXPECT_EQ(sc_root_.limit_except_.allow_all_, true);
    EXPECT_EQ(sc_root_.limit_except_.deny_.size(), 1);
    EXPECT_EQ(sc_root_.limit_except_.allow_.size(), 0);
    EXPECT_EQ(sc_root_.limit_except_.deny_[0], "172.17.0.1");
}

TEST_F(HandleLimitExceptTest, AmountOfReturnArgs2) {
    context_.main_ = v_str({"limit_except", "GET" });
    context_.directives_.push_back({"allow", "172.17.0.1", "172.17.0.2"});

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(context_));
    EXPECT_EQ(sc_root_.limit_except_.deny_all_, false);
    EXPECT_EQ(sc_root_.limit_except_.allow_all_, false);
    EXPECT_EQ(sc_root_.limit_except_.deny_.size(), 0);
    EXPECT_EQ(sc_root_.limit_except_.allow_.size(), 2);
    EXPECT_NE(std::find(sc_root_.limit_except_.allow_.begin(),
                        sc_root_.limit_except_.allow_.end(), "172.17.0.1"),
              sc_root_.limit_except_.allow_.end());
    EXPECT_NE(std::find(sc_root_.limit_except_.allow_.begin(),
                        sc_root_.limit_except_.allow_.end(), "172.17.0.2"),
              sc_root_.limit_except_.allow_.end());
}

TEST_F(HandleLimitExceptTest, LimitExceptNoHTTPMethods) {
    context_.main_ = v_str({"limit_except" });
    context_.directives_.push_back(v_str({"deny", "all"}));

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptWrongHTTPMethods) {
    context_.main_ = v_str({"limit_except", "QWER" });
    context_.directives_.push_back(v_str({"deny", "all"}));

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptCorrectAndWrongHTTPMethods) {
    context_.main_ = v_str({"limit_except", "GET", "QWER" });
    context_.directives_.push_back(v_str({"deny", "all"}));

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptButContextIsEmpty) {
    context_.main_ = v_str({"limit_except", "GET", "POST", "DELETE" });

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptButContextLacksEssentials) {
    context_.main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
    context_.directives_.push_back(v_str({"blah", "whatever"}));

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, WrongMethodKO) {
    context_.main_ = v_str({"limit_except", "zz" });
    context_.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, RepeatableMethodsKO) {
    context_.main_ = v_str({"limit_except", "GET", "GET"});
    context_.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, NoMethodsKO) {
    context_.main_ = v_str({"limit_except"});
    context_.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, NoDirective) {
    context_.main_ = v_str({"limit_except", "GET" });

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, WrongReturnCode) {
    context_.main_ = v_str({"limit_except", "GET" });
    context_.directives_.push_back({"return", "666"});

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, WrongAmountOfReturnArgs1) {
    context_.main_ = v_str({"limit_except", "GET" });
    context_.directives_.push_back({"deny"});

    EXPECT_THROW(HandleLimitExcept(context_), Limit::LimitExceptException);
}