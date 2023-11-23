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
#include "../src/Config/Config.h"
#include "../src/Server/ServerExceptions.h"


TEST(LocationTest, MarkDefinedNonMarked) {
    bool unmarked = false;
    EXPECT_NO_THROW(Location::MarkDefined("key", unmarked, v_strings({"key",
                                                                     "value"})));
    EXPECT_TRUE(unmarked);
}

TEST(LocationTest, MarkDefinedMarked) {
    bool marked = true;
    EXPECT_NO_THROW(Location::MarkDefined("key", marked, v_strings({"key", "value"})));
    EXPECT_TRUE(marked);
}

TEST(LocationTest, MarkDefinedFailsDueToInsufficientDirective) {
    bool unmarked = false;
    EXPECT_THROW(Location::MarkDefined("key", unmarked, v_strings({"key"})),
                 Location::LocationException);
}

TEST(LocationTest, UMarkDefinedFailsRepetative) {
    bool marked = true;
    EXPECT_THROW(Location::UMarkDefined("key", marked, v_strings({"key", "value"})),
                 Location::LocationException);
}

TEST(LocationTest, UMarkDefinedFailsDueToInsufficientDirective) {
    bool unmarked = false;
    EXPECT_THROW(Location::UMarkDefined("key", unmarked, v_strings({"key"})),
                 Location::LocationException);
}

TEST(LocationTest, UMarkDefinedMarked) {
    bool marked = false;
    EXPECT_NO_THROW(Location::UMarkDefined("key", marked,
                                           v_strings({"key", "value"})));
    EXPECT_TRUE(marked);
}

class SimpleLocTest : public ::testing::Test, public Location {
public:
    explicit SimpleLocTest() : Location() {};
protected:
    Location default_loc_;
};

TEST_F(SimpleLocTest, HandleRootFail) {
    EXPECT_THROW(default_loc_.HandleRoot({"root", "/aaa", "/bbb"}), LocationException);
    EXPECT_THROW(default_loc_.HandleRoot({"root"}), LocationException);
}

TEST_F(SimpleLocTest, HandleRootSuccess) {
    EXPECT_NO_THROW(default_loc_.HandleRoot({"root", "/"}));
    EXPECT_EQ(default_loc_.root_, "/");
}

class LocationWithSubsTest : public ::testing::Test, public Location {
public:
    explicit LocationWithSubsTest() : Location() {};
protected:
    Location default_loc_;

    virtual void SetUp() {
        default_loc_ = Location("/");
        Location home("/home");
        home.index_.insert("home_index.html");
        default_loc_.sublocations_.insert(home);
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
    default_loc_.AddErrorPages(v_strings({"error_page", "404", "/404.html"}));
    default_loc_.AddErrorPages(v_strings({"error_page", "403", "/403.html"}));

    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404.html", 404))->address_, "/404.html");
    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404.html", 400)), default_loc_.error_pages_.end());
}

TEST_F(LocationWithSubsTest, UpdateErrPages) {
    default_loc_.AddErrorPages(v_strings({"error_page", "404", "/404.html"}));

    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404.html", 404))->address_, "/404.html");

    default_loc_.AddErrorPages(v_strings({"error_page", "404", "/404_new.html"}));

    EXPECT_EQ(default_loc_.error_pages_.find(
            ErrPage("/404_new.html", 404))->address_, "/404_new.html");
}

TEST_F(LocationWithSubsTest, ErrPagesWrongDirective) {
    EXPECT_THROW(default_loc_.AddErrorPages(v_strings(
                                        {"error_page", "404"})),
                 LocationException);
    EXPECT_THROW(default_loc_.AddErrorPages(v_strings(
                                        {"error_page", "304"})),
                 LocationException);
    EXPECT_THROW(default_loc_.AddErrorPages(v_strings(
                                        {"error_page", "404", "/aaa", "bbb"})),
                 LocationException);
    EXPECT_THROW(default_loc_.AddErrorPages(v_strings(
                                        {"error_page", "zzz", "/aaa"})),
                 LocationException);
}

TEST_F(LocationWithSubsTest, HasSameAddressAsOneOfSublocationsOf) {
    Location contact("/contact");
    contact.index_.insert("contact_index.html");
    default_loc_.sublocations_.insert(contact);

    Location about("/about");
    about.index_.insert("about_index.html");

    EXPECT_TRUE(contact.HasSameAddressAsOneOfSublocationsOf(default_loc_));
    EXPECT_FALSE(about.HasSameAddressAsOneOfSublocationsOf(default_loc_));
}

TEST_F(LocationWithSubsTest, HandleReturnGood) {
    Location redirect_home("/go-home");
    redirect_home.HandleLocationReturn(v_strings({"return", "301", "/home"}));
    EXPECT_EQ(redirect_home.return_code_, 301);
    EXPECT_EQ(redirect_home.return_address_, "/home");

    Location forbidden("/forbidden");
    forbidden.HandleLocationReturn(v_strings({"return", "403"}));
    EXPECT_EQ(forbidden.return_code_, 403);
}

TEST_F(LocationWithSubsTest, HandleReturnWrong) {
    EXPECT_THROW(default_loc_.HandleLocationReturn(
            v_strings({"return", "301"})), LocationException);

    Location forbidden("/forbidden");
    EXPECT_THROW(forbidden.HandleLocationReturn(
                            v_strings({"return", "zzz","/home"})),
                 LocationException);
    EXPECT_THROW(forbidden.HandleLocationReturn(
                            v_strings({"return", "301","/home", "/gg"})),
                 LocationException);
}

class LocationCheckTest : public ::testing::Test, public Location {
public:
    explicit LocationCheckTest() : Location("/") {};
protected:
    Location parent_;
    ServerConfiguration conf_;
    std::vector<v_strings> directives_;

    virtual void SetUp() {
        conf_ = ServerConfiguration();
        directives_ = std::vector<v_strings>();
        conf_.port_ = 8080;
    }
};

TEST_F(LocationCheckTest, CheckRootLocTest) {
    directives_.push_back({"root", "/root"});
    directives_.push_back({"index", "index_i.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error.html"});

    EXPECT_NE(conf_.locations_.error_pages_.find(ErrPage("/htmls/404.html", 404)),
              conf_.locations_.error_pages_.end());
    EXPECT_NE(conf_.locations_.error_pages_.find(ErrPage("/htmls/403.html", 403)),
              conf_.locations_.error_pages_.end());

    EXPECT_NO_THROW(
            conf_.locations_.ProcessDirectives(directives_));

    EXPECT_EQ(conf_.locations_.root_, "/root");
    EXPECT_EQ(conf_.locations_.index_, std::set<std::string>({"index_i.html"}));

    EXPECT_NE(conf_.locations_.error_pages_.find(ErrPage("error.html", 403)),
              conf_.locations_.error_pages_.end());
    EXPECT_NE(conf_.locations_.error_pages_.find(ErrPage("error.html", 400)),
              conf_.locations_.error_pages_.end());
    EXPECT_NE(conf_.locations_.error_pages_.find(ErrPage("error.html", 416)),
              conf_.locations_.error_pages_.end());

    EXPECT_EQ(error_pages_.find(ErrPage("/htmls/404.html", 404)),
              error_pages_.end());
    EXPECT_EQ(error_pages_.find(ErrPage("/htmls/403.html", 403)),
              error_pages_.end());
}

TEST_F(LocationCheckTest, CheckNonRootLocTest) {
    address_ = "/home";
    directives_.push_back({"root", "/root"});
    directives_.push_back({"index", "index_i.html"});
    directives_.push_back({"error_page", "403", "400", "416", "error.html"});
    directives_.push_back({"return", " 300", "/home"});

    EXPECT_NO_THROW(ProcessDirectives(directives_));

    EXPECT_EQ(root_, "/root");
    EXPECT_EQ(index_, std::set<std::string>({"index_i.html"}));
    EXPECT_EQ(return_code_, 300);
    EXPECT_EQ(return_address_, "/home");

    EXPECT_NE(error_pages_.find(ErrPage("error.html", 403)),
              error_pages_.end());
    EXPECT_NE(error_pages_.find(ErrPage("error.html", 400)),
              error_pages_.end());
    EXPECT_NE(error_pages_.find(ErrPage("error.html", 416)),
              error_pages_.end());
}
