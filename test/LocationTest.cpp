/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerPreconfigTest.cpp                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/23 15:45:54 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include <algorithm>
#include "../src/Config/Config.h"
#include "../src/Server/ServerExceptions.h"


TEST(LocationTest, MarkDefinedNonMarked) {
    bool unmarked = false;
    EXPECT_NO_THROW(Location::MarkDefined("key", unmarked, v_str({"key",
                                                                  "value"})));
    EXPECT_TRUE(unmarked);
}

TEST(LocationTest, MarkDefinedMarked) {
    bool marked = true;
    EXPECT_NO_THROW(Location::MarkDefined("key", marked, v_str({"key", "value"})));
    EXPECT_TRUE(marked);
}

TEST(LocationTest, MarkDefinedFailsDueToInsufficientDirective) {
    bool unmarked = false;
    EXPECT_THROW(Location::MarkDefined("key", unmarked, v_str({"key"})),
                 Location::LocationException);
}

TEST(LocationTest, UMarkDefinedFailsRepetative) {
    bool marked = true;
    EXPECT_THROW(Location::UMarkDefined("key", marked, v_str({"key", "value"})),
                 Location::LocationException);
}

TEST(LocationTest, UMarkDefinedFailsDueToInsufficientDirective) {
    bool unmarked = false;
    EXPECT_THROW(Location::UMarkDefined("key", unmarked, v_str({"key"})),
                 Location::LocationException);
}

TEST(LocationTest, UMarkDefinedMarked) {
    bool marked = false;
    EXPECT_NO_THROW(Location::UMarkDefined("key", marked,
                                           v_str({"key", "value"})));
    EXPECT_TRUE(marked);
}

class SimpleLocTest : public ::testing::Test, public Location {
public:
    explicit SimpleLocTest() : Location() {};
protected:
    ServerConfiguration sc;
    std::vector<v_str> directives_;
};

TEST_F(SimpleLocTest, LocationWithQuestionInName) {
    EXPECT_THROW(Location("/ho?me"), Location::LocationException);
    EXPECT_THROW(Location("/ho?me", sc.GetRootIt()),Location::LocationException);
    EXPECT_THROW(Location("/home/w?tf"), Location::LocationException);
    EXPECT_THROW(Location("/ho?me/w?tf", sc.GetRootIt()),Location::LocationException);
}

TEST_F(SimpleLocTest, ProcessDirectivesTestForNewLocation) {
    Location home_loc = Location("/home", sc.GetRootIt());

    directives_.push_back({"root", "/root"});
    directives_.push_back({"index", "index_i.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error.html"});
    directives_.push_back({"return", "301", "http://localhost:4280/home"});

    EXPECT_NO_THROW(home_loc.ProcessDirectives(directives_));

    EXPECT_EQ(home_loc.root_, "/root");
    EXPECT_NE(std::find(home_loc.index_.begin(), home_loc.index_.end(),"index_i.html"), home_loc.index_.end());
    EXPECT_EQ(home_loc.return_code_, 301);
//    EXPECT_EQ(home_loc.return_address_, "/home");

    EXPECT_NE(home_loc.error_pages_.find(ErrPage("error.html", 403)),
              home_loc.error_pages_.end());
    EXPECT_NE(home_loc.error_pages_.find(ErrPage("error.html", 400)),
              home_loc.error_pages_.end());
    EXPECT_NE(home_loc.error_pages_.find(ErrPage("error.html", 416)),
              home_loc.error_pages_.end());
}

TEST_F(SimpleLocTest, ProcessDirectivesTestForRootRedefinition) {
    directives_.push_back({"root", "/root"});
    directives_.push_back({"index", "index_i.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error.html"});

    Location &root_loc = sc.GetRoot();

    EXPECT_NO_THROW(root_loc.ProcessDirectives(directives_));
    
    EXPECT_EQ(root_loc.root_, "/root");
    EXPECT_NE(std::find(root_loc.index_.begin(), root_loc.index_.end(),"index_i.html"), root_loc.index_.end());

    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error.html", 403)),
              root_loc.error_pages_.end());
    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error.html", 400)),
              root_loc.error_pages_.end());
    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error.html", 416)),
              root_loc.error_pages_.end());
}

// todo is it even possible? makes sense?
TEST_F(SimpleLocTest, ProcessDirectivesTestForRootRootRedefinition) {
    std::vector<v_str> directives_2;

    directives_.push_back({"root", "/root1"});
    directives_.push_back({"index", "index_1.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error1.html"});

    directives_2.push_back({"root", "/root2"});
    directives_2.push_back({"index", "index_2.html"});
    directives_2.push_back({"error_page", "403", "400", "416", "error2.html"});

    Location &root_loc = sc.GetRoot();

    EXPECT_NO_THROW(root_loc.ProcessDirectives(directives_));

    EXPECT_EQ(root_loc.root_, "/root1");
    EXPECT_NE(std::find(root_loc.index_.begin(), root_loc.index_.end(),"index_1.html"), root_loc.index_.end());

    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error1.html", 403)),
              root_loc.error_pages_.end());
    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error1.html", 400)),
              root_loc.error_pages_.end());
    EXPECT_NE(root_loc.error_pages_.find(ErrPage("error1.html", 416)),
              root_loc.error_pages_.end());

    EXPECT_NO_THROW(root_loc.ProcessDirectives(directives_2));

    Location &root_loc2 = sc.GetRoot();

    EXPECT_EQ(sc.locations_.size(), 1);
    EXPECT_EQ(root_loc2.sublocations_.size(), 0);

    EXPECT_EQ(root_loc2.root_, "/root2");
    EXPECT_NE(std::find(root_loc2.index_.begin(), root_loc2.index_.end(),"index_2.html"), root_loc2.index_.end());

    EXPECT_NE(root_loc2.error_pages_.find(ErrPage("error2.html", 403)),
              root_loc2.error_pages_.end());
    EXPECT_NE(root_loc2.error_pages_.find(ErrPage("error2.html", 400)),
              root_loc2.error_pages_.end());
    EXPECT_NE(root_loc2.error_pages_.find(ErrPage("error2.html", 416)),
              root_loc2.error_pages_.end());
}

// todo can we redefine non-root?
//TEST_F(SimpleLocTest, ProcessDirectivesTestForNonRootRedefinition) {
//    Location default_loc = Location("/", sc.GetRootIt());
//
//    directives_.push_back({"root", "/root"});
//    directives_.push_back({"index", "index_i.html"});
//    directives_.push_back({"error_page", "403", "400", "416", "error.html"});
//    directives_.push_back({"return", " 300", "/home"});
//
//    v_str root_index_update;
//
//    EXPECT_NO_THROW(root_index_update =
//                                    default_loc.ProcessDirectives(directives_));
//
//    EXPECT_EQ(default_loc.root_, "/root");
//    EXPECT_EQ(default_loc.index_, std::set<std::string>({"index_i.html"}));
//    EXPECT_EQ(default_loc.return_code_, 300);
//    EXPECT_EQ(default_loc.return_address_, "/home");
//
//    EXPECT_NE(error_pages_.find(ErrPage("error.html", 403)),
//              error_pages_.end());
//    EXPECT_NE(error_pages_.find(ErrPage("error.html", 400)),
//              error_pages_.end());
//    EXPECT_NE(error_pages_.find(ErrPage("error.html", 416)),
//              error_pages_.end());
//
//    EXPECT_EQ(root_index_update, v_str({ "index", "index_i.html"}));
//}



class LocationWithSubsTest : public ::testing::Test, public Location {
public:
    explicit LocationWithSubsTest() : Location() {};
protected:
    Location default_loc_;

    virtual void SetUp() {
        default_loc_ = Location("/");
        Location home("/home");
        home.index_.push_back("home_index.html");
        default_loc_.sublocations_.push_back(home);
    }
};

TEST_F(LocationWithSubsTest, NonExistingFindSublocationByAddressTest) {
    EXPECT_THROW(default_loc_.FindSublocationByAddress(""),
                 NotFoundException);
}

TEST_F(LocationWithSubsTest, ExistingFindSublocationByAddressTest) {
    const Location & loc = default_loc_.FindSublocationByAddress("/home");

    EXPECT_EQ(loc.address_, "/home");
    EXPECT_EQ(*(loc.index_.begin()), "home_index.html");
}

TEST_F(LocationWithSubsTest, RootFindSublocationByAddressTest) {
    const Location & loc = default_loc_.FindSublocationByAddress("/");

    EXPECT_EQ(loc.address_, "/");
}

TEST_F(LocationWithSubsTest, AddErrPages) {
    default_loc_.AddErrorPages(v_str({"error_page", "404", "/404.html"}));
    default_loc_.AddErrorPages(v_str({"error_page", "403", "/403.html"}));

    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404.html", 404))->address_, "/404.html");
    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404.html", 400)), default_loc_.error_pages_.end());
}

TEST_F(LocationWithSubsTest, UpdateErrPages) {
    default_loc_.AddErrorPages(v_str({"error_page", "404", "/404.html"}));

    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404.html", 404))->address_, "/404.html");

    default_loc_.AddErrorPages(v_str({"error_page", "404", "/404_new.html"}));

    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404_new.html", 404))->address_, "/404_new.html");
}

TEST_F(LocationWithSubsTest, ErrPagesWrongDirective) {
    EXPECT_THROW(default_loc_.AddErrorPages(v_str(
                                        {"error_page", "404"})),
                 LocationException);
    EXPECT_THROW(default_loc_.AddErrorPages(v_str(
                                        {"error_page", "304"})),
                 LocationException);
    EXPECT_THROW(default_loc_.AddErrorPages(v_str(
                                        {"error_page", "404", "/aaa", "bbb"})),
                 LocationException);
    EXPECT_THROW(default_loc_.AddErrorPages(v_str(
                                        {"error_page", "zzz", "/aaa"})),
                 LocationException);
}

TEST_F(LocationWithSubsTest, HasSameAddressAsOneOfSublocationsOf) {
    Location contact("/contact");
    contact.index_.push_back("contact_index.html");
    default_loc_.sublocations_.push_back(contact);

    Location about("/about");
    about.index_.push_back("about_index.html");

    EXPECT_TRUE(contact.HasSameAddressAsOneOfSublocationsOf(default_loc_));
    EXPECT_FALSE(about.HasSameAddressAsOneOfSublocationsOf(default_loc_));
}

TEST_F(LocationWithSubsTest, HandleReturnGood) {
    Location redirect_home("/go-home");
    redirect_home.HandleLocationReturn(v_str({"return", "301", "http://localhost:4280/home"}));
    EXPECT_EQ(redirect_home.return_code_, 301);
    EXPECT_EQ(redirect_home.return_address_, "http://localhost:4280/home");

    Location forbidden("/forbidden");
    forbidden.HandleLocationReturn(v_str({"return", "403"}));
    EXPECT_EQ(forbidden.return_code_, 403);
}

TEST_F(LocationWithSubsTest, HandleReturnWrong) {
    Location forbidden("/forbidden");
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "zzz", "http://localhost:4280/home"})),
                 LocationException);
    EXPECT_THROW(forbidden.HandleLocationReturn(
            v_str({"return", "301", "http://localhost:4280/home", "/gg"})),
                 LocationException);
}

class LocationCheckTest : public ::testing::Test, public Location {
public:
    explicit LocationCheckTest() : Location("/") {};
protected:
    Location parent_;
    ServerConfiguration conf_;
    std::vector<v_str> directives_;

    virtual void SetUp() {
        conf_ = ServerConfiguration();
        directives_ = std::vector<v_str>();
        conf_.port_ = 8080;
    }
};

//TEST_F(LocationCheckTest, CheckRootLocTest) {
//    directives_.push_back({"root", "/root"});
//    directives_.push_back({"index", "index_i.html"});
//    directives_.push_back({"error_page", "403", "400", "416", "error.html"});
//
//    EXPECT_NE(conf_.GetRoot().error_pages_.find(ErrPage("/htmls/404.html", 404)),
//              conf_.GetRoot().error_pages_.end());
//    EXPECT_NE(conf_.GetRoot().error_pages_.find(ErrPage("/htmls/403.html", 403)),
//              conf_.GetRoot().error_pages_.end());
//
//    EXPECT_NO_THROW(
//            conf_.GetRoot().ProcessDirectives(directives_));
//
//    EXPECT_EQ(conf_.GetRoot().root_, "/root");
////    todo replace, not appnd !
//    EXPECT_NE(std::find(conf_.GetRoot().index_.begin(), conf_.GetRoot().index_.end(),"index_i.html"),
//              conf_.GetRoot().index_.end());
//
//    EXPECT_NE(conf_.GetRoot().error_pages_.find(ErrPage("error.html", 403)),
//              conf_.GetRoot().error_pages_.end());
//    EXPECT_NE(conf_.GetRoot().error_pages_.find(ErrPage("error.html", 400)),
//              conf_.GetRoot().error_pages_.end());
//    EXPECT_NE(conf_.GetRoot().error_pages_.find(ErrPage("error.html", 416)),
//              conf_.GetRoot().error_pages_.end());
//
//    EXPECT_EQ(error_pages_.find(ErrPage("/htmls/404.html", 404)),
//              error_pages_.end());
//    EXPECT_EQ(error_pages_.find(ErrPage("/htmls/403.html", 403)),
//              error_pages_.end());
//}

TEST_F(LocationCheckTest, CheckNonRootLocTest) {
    address_ = "/home";
    directives_.push_back({"root", "/root"});
    directives_.push_back({"index", "index_i.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error.html"});
    directives_.push_back({"return", "301", "http://localhost:4280/home"});

    EXPECT_NO_THROW(ProcessDirectives(directives_));

    EXPECT_EQ(root_, "/root");
    EXPECT_EQ(index_, l_str({"index_i.html"}));
    EXPECT_EQ(return_code_, 301);
    EXPECT_EQ(return_address_, "http://localhost:4280/home");

    EXPECT_NE(error_pages_.find(ErrPage("error.html", 403)),
              error_pages_.end());
    EXPECT_NE(error_pages_.find(ErrPage("error.html", 400)),
              error_pages_.end());
    EXPECT_NE(error_pages_.find(ErrPage("error.html", 416)),
              error_pages_.end());
}
