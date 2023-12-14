#include <gtest/gtest.h>
#include "../../../../../src/Config/Node.h"
#include "../../../../../src/Config/server_configuration/ServerConfiguration.h"

class CheckCorrectLocationContextTest : public ::testing::Test,
                                        public ServerConfiguration {
public:
    explicit CheckCorrectLocationContextTest() : ServerConfiguration() {};
protected:
    Node context_;
};

TEST_F(CheckCorrectLocationContextTest, ComponentsTestLocationBlockEmpty) {
    context_.main_ = v_str({"location", "/" });

    EXPECT_THROW(CheckLocationContextIsCorrect(context_), ServerConfigurationException);
}

TEST_F(CheckCorrectLocationContextTest, ComponentsTestLocationBlockWithoutPath) {
    context_.main_ = v_str({"location"});
    context_.directives_.push_back(v_str({"root", "/some/where"}));

    EXPECT_THROW(CheckLocationContextIsCorrect(context_), ServerConfigurationException);
}

TEST_F(CheckCorrectLocationContextTest, ComponentsTestAllRootInsideLocation) {
    context_.main_ = v_str({"location", "/" });
    context_.directives_.push_back(v_str({"root", "/some/where"}));

    EXPECT_NO_THROW(CheckLocationContextIsCorrect(context_));
}

TEST_F(CheckCorrectLocationContextTest, ComponentsTestAllIndexInsideLocation) {
    context_.main_ = v_str({"location", "/" });
    context_.directives_.push_back(v_str({"index", "index.html", "index.htm" }));

    EXPECT_NO_THROW(CheckLocationContextIsCorrect(context_));
}
