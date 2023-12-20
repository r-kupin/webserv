/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    HandleSingleStepLocation.cpp                       :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/17 14:11:46 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include <algorithm>
#include "../../../../../src/Config/server_configuration/ServerConfiguration.h"

class HandleSingleStepLocation  : public ::testing::Test, public ServerConfiguration {
public:
    explicit HandleSingleStepLocation() : ServerConfiguration() {};
protected:
    Node context_;
    Node location_root_;

    virtual void SetUp() {
        location_root_ = Node();

        location_root_.main_ = v_str({"location", "/"});
        location_root_.directives_.push_back({"listen", "8080"});
        location_root_.directives_.push_back({"server_name", "example.com"});
        location_root_.directives_.push_back({"client_max_body_size", "2048"});
        location_root_.directives_.push_back({"error_page", "404", "/404.html"});
        location_root_.directives_.push_back({"error_page", "500", "502",
                                              "503", "504", "/50x.html"});
    }
};

TEST_F(HandleSingleStepLocation, RootSubnodeParentTest) {
    context_.main_ = v_str({"location", "/loc_X" });
    context_.directives_.push_back({"index", "index.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    context_.child_nodes_.push_back(limit_except_get);

    EXPECT_NO_THROW(HandleLocationContext(context_));

    EXPECT_EQ(locations_.size(), 1);
    EXPECT_EQ(locations_.begin(), GetRootIt());
    EXPECT_EQ(GetRoot().sublocations_.size(), 1);
    EXPECT_EQ(GetRoot().sublocations_.begin()->parent_, GetRootIt());

    Location loc_loc = GetRoot().sublocations_.back();

    EXPECT_EQ(loc_loc.address_, "/loc_X");
    EXPECT_NE(std::find(loc_loc.index_.begin(),
                        loc_loc.index_.end(),
                        "index.html"),
              loc_loc.index_.end());
}

TEST_F(HandleSingleStepLocation, WithLimitExcept) {
    context_.main_ = v_str({"location", "/loc_X" });
    context_.directives_.push_back({"index", "index.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    context_.child_nodes_.push_back(limit_except_get);

    EXPECT_NO_THROW(HandleLocationContext(context_));

    Location loc_loc = GetRoot().sublocations_.back();
    EXPECT_EQ(loc_loc.limit_except_.except_, std::set<Methods>({GET}));
}

TEST_F(HandleSingleStepLocation, HomeInReDefinedRoot) {

    context_.main_ = v_str({"location", "/loc_X" });
    context_.directives_.push_back({"index", "index.html"});

    location_root_.child_nodes_.push_back(context_);

    EXPECT_NO_THROW(HandleLocationContext(location_root_));

    EXPECT_EQ(GetRoot().address_, "/");
    EXPECT_EQ(GetRoot().root_ , "resources/root_loc_default");

    const std::set<ErrPage>::iterator &NotFoundErrPage =
            GetRoot().error_pages_.find(ErrPage("/404.html", 404));
    EXPECT_NE(NotFoundErrPage, GetRoot().error_pages_.end());
    EXPECT_EQ(NotFoundErrPage->address_, "/404.html");

    const std::set<ErrPage>::iterator &InternalServerError =
            GetRoot().error_pages_.find(ErrPage("/50x.html", 500));
    EXPECT_NE(InternalServerError, GetRoot().error_pages_.end());
    EXPECT_EQ(InternalServerError->address_, "/50x.html");

    EXPECT_EQ(GetRoot().return_code_ , 0);
    EXPECT_EQ(GetRoot().return_address_ , "");

    EXPECT_EQ(GetRoot().sublocations_.begin()->address_, "/loc_X");
    EXPECT_NE(std::find(GetRoot().sublocations_.begin()->index_.begin(),
                        GetRoot().sublocations_.begin()->index_.end(),
                        "index.html"),
              GetRoot().sublocations_.begin()->index_.end());
}

TEST_F(HandleSingleStepLocation, MultipleLimitExcept) {
    context_.main_ = v_str({"location", "/loc_X" });
    context_.directives_.push_back({"index", "index.html"});

    Node limit_except_get;
    limit_except_get.main_ = v_str({"limit_except", "GET" });
    limit_except_get.directives_.push_back({"deny", "all"});

    Node limit_except_post;
    limit_except_post.main_ = v_str({"limit_except", "POST" });
    limit_except_post.directives_.push_back({"deny", "all"});

    context_.child_nodes_.push_back(limit_except_get);
    context_.child_nodes_.push_back(limit_except_post);

    EXPECT_THROW(HandleLocationContext(context_), Location::LocationException);
}

TEST_F(HandleSingleStepLocation, MultipleAddressesUnderTheSameParent) {
    Node loc_n1;
    loc_n1.main_ = v_str({"location", "/loc_X" });
    loc_n1.directives_.push_back({"index", "index.html"});

    Node loc_n2;
    loc_n2.main_ = v_str({"location", "/loc_X" });
    loc_n2.directives_.push_back({"return", "301",
                                  "http://localhost:4280/somewhere"});

    location_root_.child_nodes_.push_back(loc_n2);
    location_root_.child_nodes_.push_back(loc_n1);

    EXPECT_THROW(HandleLocationContext(location_root_), ServerConfigurationException);
}
