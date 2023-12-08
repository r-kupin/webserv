#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../src/Config/ConfigSubmodules.h"

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

    v_str               main_;
    std::vector<v_str>  directives_;
};

TEST_F(HandleLimitExceptTest, LimitExceptCorrectHTTPMethods) {
    main_ = v_str({"limit_except", "GET" });
    directives_.push_back(v_str({"deny", "all"}));

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(main_, directives_));
    EXPECT_TRUE(sc_root_.limit_except_.deny_all_);
    EXPECT_FALSE(sc_root_.limit_except_.allow_all_);
}

TEST_F(HandleLimitExceptTest, LimitExceptAllCorrectHTTPMethods) {
    main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
    directives_.push_back(v_str({"allow", "all"}));

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(main_, directives_));
    EXPECT_FALSE(sc_root_.limit_except_.deny_all_);
    EXPECT_TRUE(sc_root_.limit_except_.allow_all_);
}

TEST_F(HandleLimitExceptTest, TwoDirectives) {
    main_ = v_str({"limit_except", "GET" });
    directives_.push_back({"deny", "all"});
    directives_.push_back({"allow", "172.17.0.1"});

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(main_, directives_));
    EXPECT_EQ(sc_root_.limit_except_.deny_all_, true);
    EXPECT_EQ(sc_root_.limit_except_.allow_all_, false);
    EXPECT_EQ(sc_root_.limit_except_.deny_.size(), 0);
    EXPECT_EQ(sc_root_.limit_except_.allow_.size(), 1);
    EXPECT_EQ(sc_root_.limit_except_.allow_[0], "172.17.0.1");
}

TEST_F(HandleLimitExceptTest, TwoDirectives2) {
    main_ = v_str({"limit_except", "GET" });
    directives_.push_back({"allow", "all"});
    directives_.push_back({"deny", "172.17.0.1"});

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(main_, directives_));
    EXPECT_EQ(sc_root_.limit_except_.deny_all_, false);
    EXPECT_EQ(sc_root_.limit_except_.allow_all_, true);
    EXPECT_EQ(sc_root_.limit_except_.deny_.size(), 1);
    EXPECT_EQ(sc_root_.limit_except_.allow_.size(), 0);
    EXPECT_EQ(sc_root_.limit_except_.deny_[0], "172.17.0.1");
}

TEST_F(HandleLimitExceptTest, AmountOfReturnArgs2) {
    main_ = v_str({"limit_except", "GET" });
    directives_.push_back({"allow", "172.17.0.1", "172.17.0.2"});

    EXPECT_NO_THROW(sc_root_.HandleLimitExcept(main_, directives_));
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
    main_ = v_str({"limit_except" });
    directives_.push_back(v_str({"deny", "all"}));

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptWrongHTTPMethods) {
    main_ = v_str({"limit_except", "QWER" });
    directives_.push_back(v_str({"deny", "all"}));

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptCorrectAndWrongHTTPMethods) {
    main_ = v_str({"limit_except", "GET", "QWER" });
    directives_.push_back(v_str({"deny", "all"}));

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptButContextIsEmpty) {
    main_ = v_str({"limit_except", "GET", "POST", "DELETE" });

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, LimitExceptButContextLacksEssentials) {
    main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
    directives_.push_back(v_str({"blah", "whatever"}));

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, WrongMethodKO) {
    main_ = v_str({"limit_except", "zz" });
    directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, RepeatableMethodsKO) {
    main_ = v_str({"limit_except", "GET", "GET"});
    directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, NoMethodsKO) {
    main_ = v_str({"limit_except"});
    directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, NoDirective) {
    main_ = v_str({"limit_except", "GET" });

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, WrongReturnCode) {
    main_ = v_str({"limit_except", "GET" });
    directives_.push_back({"return", "666"});

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

TEST_F(HandleLimitExceptTest, WrongAmountOfReturnArgs1) {
    main_ = v_str({"limit_except", "GET" });
    directives_.push_back({"deny"});

    EXPECT_THROW(HandleLimitExcept(main_, directives_),
                 Limit::LimitExceptException);
}

//
//TEST_F(HandleLimitExceptTest, LimitExceptWrongInServerContext) {
//    Node limit_except;
//
//    limit_except.main_ = v_str({"limit_except", "GET", "POST" });
//    limit_except.directives_.push_back(v_str({"deny", "all"}));
//
//    server_.child_nodes_.push_back(limit_except);
//    root_.child_nodes_.push_back(server_);
//    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
//}
