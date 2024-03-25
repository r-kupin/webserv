/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    UtilsTest.cpp                                      :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/01/04 16:45:06 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../../../src/utils/Utils.h"

TEST(CheckFS, CheckFilesystemExist) {
    std::string def_res_address_ = "test_resources/nested_locations";
    EXPECT_EQ(Utils::CheckFilesystem(def_res_address_ + "/nginx.conf"), COMM_FILE);
    EXPECT_EQ(Utils::CheckFilesystem(def_res_address_ + "/www/X"), NOTHING);
    EXPECT_EQ(Utils::CheckFilesystem(def_res_address_ + "/www"), DIRECTORY);
}
