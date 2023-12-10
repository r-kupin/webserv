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
#include "../../../../../src/Config/config/Config.h"

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