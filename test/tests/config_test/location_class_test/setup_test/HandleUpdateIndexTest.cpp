#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/config/Config.h"


class IndexHandlerTest : public ::testing::Test, public Location {
public:
    explicit IndexHandlerTest()
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