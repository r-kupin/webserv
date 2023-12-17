/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    HandleMultistepLocation.cpp                        :+:      :+:    :+:  */
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

class HandleMultistepLocationTest : public ::testing::Test,
                                     public ServerConfiguration {};