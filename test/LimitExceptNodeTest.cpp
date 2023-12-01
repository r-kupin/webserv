/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    HandleLimitExceptContext.cpp                       :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/07 17:55:45 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include <algorithm>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"

class LimitExceptNodeTest : public ::testing::Test, public Config {
public:
    explicit LimitExceptNodeTest() : Config() {};
protected:
    ServerConfiguration conf_;
    Node location_root_;
    Node limit_except;
    Limit test;

    virtual void SetUp() {
        location_root_ = Node();
        conf_ = ServerConfiguration();
        limit_except = Node();
        test = Limit();

        location_root_.main_ = v_strings({"location", "/"});
        location_root_.directives_.push_back({"listen", "8080"});
        location_root_.directives_.push_back({"server_name", "example.com"});
        location_root_.directives_.push_back({"client_max_body_size", "2048"});
        location_root_.directives_.push_back({"error_page", "404", "/404.html"});
        location_root_.directives_.push_back({"error_page", "500", "502",
                                              "503", "504", "/50x.html"});
    }
};

TEST_F(LimitExceptNodeTest, WrongMethodKO) {
    limit_except.main_ = v_strings({"limit_except", "zz" });
    limit_except.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, RepeatableMethodsKO) {
    limit_except.main_ = v_strings({"limit_except", "GET","GET"});
    limit_except.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, NoMethodsKO) {
    limit_except.main_ = v_strings({"limit_except"});
    limit_except.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, TwoDirectives) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"deny", "all"});
    limit_except.directives_.push_back({"allow", "172.17.0.1"});

    EXPECT_NO_THROW(HandleLimitExceptContext(limit_except, test));
    EXPECT_EQ(test.deny_all_, true);
    EXPECT_EQ(test.allow_all_, false);
    EXPECT_EQ(test.deny_.size(), 0);
    EXPECT_EQ(test.allow_.size(), 1);
    EXPECT_EQ(test.allow_[0], "172.17.0.1");
}

TEST_F(LimitExceptNodeTest, TwoDirectives2) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"allow", "all"});
    limit_except.directives_.push_back({"deny", "172.17.0.1"});

    EXPECT_NO_THROW(HandleLimitExceptContext(limit_except, test));
    EXPECT_EQ(test.deny_all_, false);
    EXPECT_EQ(test.allow_all_, true);
    EXPECT_EQ(test.deny_.size(), 1);
    EXPECT_EQ(test.allow_.size(), 0);
    EXPECT_EQ(test.deny_[0], "172.17.0.1");
}

TEST_F(LimitExceptNodeTest, NoDirective) {
    limit_except.main_ = v_strings({"limit_except", "GET" });

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, WrongReturnCode) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"return", "666"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, WrongAmountOfReturnArgs1) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"deny"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, AmountOfReturnArgs2) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"allow", "172.17.0.1", "172.17.0.2"});

    EXPECT_NO_THROW(HandleLimitExceptContext(limit_except, test));
    EXPECT_EQ(test.deny_all_, false);
    EXPECT_EQ(test.allow_all_, false);
    EXPECT_EQ(test.deny_.size(), 0);
    EXPECT_EQ(test.allow_.size(), 2);
    EXPECT_NE(std::find(test.allow_.begin(), test.allow_.end(), "172.17.0.1"),
               test.allow_.end());
    EXPECT_NE(std::find(test.allow_.begin(), test.allow_.end(), "172.17.0.2"),
               test.allow_.end());
}