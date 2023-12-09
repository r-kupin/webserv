#include <gtest/gtest.h>
#include "../../../src/Config/Config.h"
#include "../../../src/Config/ConfigExceptions.h"

class HandleLocationContextTest  : public ::testing::Test, public Config {
public:
    explicit HandleLocationContextTest() : Config() {};
protected:
    Node root_;
    Node server_;
    Node location_;

    virtual void SetUp() {
        root_ = Node();
        server_ = Node();
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
    }
};

TEST_F(HandleLocationContextTest, ComponentsTestLocationBlockEmpty) {
    location_.main_ = v_str({"location", "/" });
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(HandleLocationContextTest, ComponentsTestLocationBlockWithoutPath) {
    location_.main_ = v_str({"location"});
    location_.directives_.push_back(v_str({"root", "/some/where"}));
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_THROW(CreateSrvConfigs(root_), ConfigFileSyntaxError);
}

TEST_F(HandleLocationContextTest, ComponentsTestAllRootInsideLocation) {
    location_.main_ = v_str({"location", "/" });
    location_.directives_.push_back(v_str({"root", "/some/where"}));
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CreateSrvConfigs(root_));
}

TEST_F(HandleLocationContextTest, ComponentsTestAllIndexInsideLocation) {
    location_.main_ = v_str({"location", "/" });
    location_.directives_.push_back(
            v_str({"index", "index.html", "index.htm" }));
    server_.child_nodes_.push_back(location_);

    root_.child_nodes_.push_back(server_);
    EXPECT_NO_THROW(CreateSrvConfigs(root_));
}
