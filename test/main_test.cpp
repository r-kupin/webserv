/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    main_test.cpp                                      :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/02/27 00:01:13 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>

int	main(int ac, char** av) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}