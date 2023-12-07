#include <gtest/gtest.h>
#include "../../../src/Config/Config.h"
#include "../../../src/Config/ConfigExceptions.h"

class LimitExceptContextTest  : public ::testing::Test, public Config {
public:
    explicit LimitExceptContextTest() : Config() {};
protected:
    Node root_;
    Node server_;
    Node location_;
    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
        location_ = Node();

        root_.main_ = v_str ({"main"});
        server_.main_ = v_str ({"server"});
        server_.directives_.push_back(
                v_str({"server_name", "localhost" }));
        server_.directives_.push_back(
                v_str({"listen", "8080" }));
        server_.directives_.push_back(
                v_str({"root", "/some/where/deep/inside" }));
        server_.directives_.push_back(
                v_str({"index", "index.html", "index.htm" }));
        server_.directives_.push_back(
                v_str({"error_page", "401" , "err.html" }));

        location_.main_ = v_str({"location", "/" });
        location_.directives_.push_back(v_str({"root", "/some/where"}));
    }
};

// ????????????
TEST_F(LimitExceptContextTest, ComponentsTestLocationSomeRandomBlocksArePresent) {
Node random;
random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

root_.child_nodes_.push_back(server_);
EXPECT_NO_THROW(CreateSrvConfigs(root_));
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptNoHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except" });
limit_except.directives_.push_back(v_str({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptWrongHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "QWER" });
limit_except.directives_.push_back(v_str({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptWrongInServerContext) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "GET", "POST" });
limit_except.directives_.push_back(v_str({"deny", "all"}));

server_.child_nodes_.push_back(limit_except);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptCorrectHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "GET" });
limit_except.directives_.push_back(v_str({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_NO_THROW(CreateSrvConfigs(root_));
}

TEST_F(LimitExceptContextTest,
       ComponentsTestLimitExceptCorrectAndWrongHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "GET", "QWER" });
limit_except.directives_.push_back(v_str({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptAllCorrectHTTPMethods) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
limit_except.directives_.push_back(v_str({"deny", "all"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_NO_THROW(CreateSrvConfigs(root_));
}

TEST_F(LimitExceptContextTest, ComponentsTestLimitExceptButContextIsEmpty) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(LimitExceptContextTest,
       ComponentsTestLimitExceptButContextLacksEssentials) {
Node random;
Node limit_except;

limit_except.main_ = v_str({"limit_except", "GET", "POST", "DELETE" });
limit_except.directives_.push_back(v_str({"bias", "whatever"}));
location_.child_nodes_.push_back(limit_except);

random.main_ = v_str({"random" });
server_.child_nodes_.push_back(random);

server_.child_nodes_.push_back(location_);
root_.child_nodes_.push_back(server_);
EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}