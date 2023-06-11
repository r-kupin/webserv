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

TEST_F(LimitExceptNodeTest, ReturnOkOneMethod) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"return", "403"});

    EXPECT_NO_THROW(HandleLimitExceptContext(limit_except, test));
    EXPECT_EQ(test.return_code_, 403);
    EXPECT_EQ(test.return_address_, "");
    EXPECT_EQ(*(test.except_.begin()), GET);
}

TEST_F(LimitExceptNodeTest, ReturnOk2MethodsRetAddr) {
    limit_except.main_ = v_strings({"limit_except", "GET", "POST" });
    limit_except.directives_.push_back({"return", "403", "/somewhere"});

    EXPECT_NO_THROW(HandleLimitExceptContext(limit_except, test));
    EXPECT_EQ(test.return_code_, 403);
    EXPECT_EQ(test.return_address_, "/somewhere");
    EXPECT_NE(test.except_.find(GET), test.except_.end());
    EXPECT_NE(test.except_.find(POST), test.except_.end());
}

TEST_F(LimitExceptNodeTest, DenyAllDelete) {
    limit_except.main_ = v_strings({"limit_except", "DELETE" });
    limit_except.directives_.push_back({"deny", "all"});

    EXPECT_NO_THROW(HandleLimitExceptContext(limit_except, test));
    EXPECT_EQ(test.return_code_, 403);
    EXPECT_EQ(test.return_address_, "");
    EXPECT_NE(test.except_.find(DELETE), test.except_.end());
}

TEST_F(LimitExceptNodeTest, WrongMethodKO) {
    limit_except.main_ = v_strings({"limit_except", "GET", "zz" });
    limit_except.directives_.push_back({"return", "403", "/somewhere"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, RepeatableMethodsKO) {
    limit_except.main_ = v_strings({"limit_except", "GET","GET"});
    limit_except.directives_.push_back({"return", "403", "/somewhere"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, NoMethodsKO) {
    limit_except.main_ = v_strings({"limit_except"});
    limit_except.directives_.push_back({"return", "403", "/somewhere"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, TwoDirectives) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"return", "403", "/somewhere"});
    limit_except.directives_.push_back({"deny", "all"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
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
    limit_except.directives_.push_back({"return"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, WrongAmountOfReturnArgs2) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"return", "404", "aa", "bb"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}

TEST_F(LimitExceptNodeTest, WrongReturnAddr) {
    limit_except.main_ = v_strings({"limit_except", "GET" });
    limit_except.directives_.push_back({"return", "404", "aa"});

    EXPECT_THROW(HandleLimitExceptContext(limit_except, test), ConfigFileSyntaxError);
}
