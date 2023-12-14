#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"
#include "../../../../../src/Config/config/ConfigExceptions.h"

class HandleDirectivesTest : public ::testing::Test, public ServerConfiguration {
public:
    explicit HandleDirectivesTest() : ServerConfiguration() {};
protected:
    Node context_;

    virtual void SetUp() {
        context_.main_ = v_str({"server"});
    }
};

TEST_F(HandleDirectivesTest, ComponentsTestNoServersFail) {
    EXPECT_THROW(ProcessDirectives(context_.directives_), ServerConfigurationException);
}

TEST_F(HandleDirectivesTest, ComponentsTestOKServerWorks) {
    context_.directives_.push_back(v_str({"server_name", "example.com"}));
    context_.directives_.push_back(v_str({"listen", "8182"}));
    context_.directives_.push_back(v_str({"root", "/not/depends/on/config"}));
    context_.directives_.push_back(v_str({"client_max_body_size", "2048"}));
    context_.directives_.push_back(v_str({"index", "index.html", "index.php"}));
    context_.directives_.push_back(v_str({"error_page", "401", "err.html"}));

    EXPECT_NO_THROW(ProcessDirectives(context_.directives_));
    EXPECT_FALSE(default_hostname_);
    EXPECT_NE(server_names_.find("example.com"), server_names_.end());
    EXPECT_EQ(port_, 8182);
    EXPECT_EQ(GetRoot().root_, "/not/depends/on/config");
    EXPECT_EQ(GetRoot().index_.size(), 2);
    EXPECT_NE(std::find(GetRoot().index_.begin(),
                        GetRoot().index_.end(),
                        "index.html"), GetRoot().index_.end());
    EXPECT_NE(std::find(GetRoot().index_.begin(),
                        GetRoot().index_.end(),
                        "index.php"), GetRoot().index_.end());
    EXPECT_TRUE(GetRoot().index_defined_);
    EXPECT_EQ(GetRoot().error_pages_.size(), 1);
    EXPECT_EQ(GetRoot().error_pages_.begin()->code_, 401);
    EXPECT_EQ(GetRoot().error_pages_.begin()->address_, "err.html");
}

TEST_F(HandleDirectivesTest, CheckServerTestHasNoPortKO) {
    context_.directives_.push_back({"root", "/not/depends/on/config"});
    EXPECT_THROW(ProcessDirectives(context_.directives_),
                 ServerConfigurationException);
}

TEST_F(HandleDirectivesTest, CheckServerTestHasOnlyPortOK) {
    context_.directives_.push_back({"listen", "8080"});
    EXPECT_NO_THROW(ProcessDirectives(context_.directives_));
}

TEST_F(HandleDirectivesTest, ServetThrowsIfMultipleListen) {
    context_.directives_.push_back({"listen", "8080"});
    context_.directives_.push_back({"listen", "8081"});
    EXPECT_THROW(ProcessDirectives(context_.directives_),
                 ServerConfigurationException);
}

TEST_F(HandleDirectivesTest, ServetThrowsIfMultipleRoots) {
    context_.directives_.push_back({"root", "/not/not/depends/on/config"});
    context_.directives_.push_back({"root", "/other/not/depends/on/config"});
    EXPECT_THROW(ProcessDirectives(context_.directives_),
                 ServerConfigurationException);
}

TEST_F(HandleDirectivesTest, ServerConfDirectivesMultipleBodySize) {
    context_.directives_.push_back({"listen", "8081"});
    context_.directives_.push_back({"server_name", "localhost"});
    context_.directives_.push_back({"client_max_body_size", "2048"});
    context_.directives_.push_back({"client_max_body_size", "10204"});
    context_.directives_.push_back({"error_page", "404", "/404.html"});
    context_.directives_.push_back({"error_page", "500", "502", "503", "504",
                                    "/50x.html"});

    EXPECT_THROW(ProcessDirectives(context_.directives_),
                 ServerConfigurationException);
}
