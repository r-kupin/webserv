/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerTest.cpp                                     :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/06/09 02:47:46 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <gtest/gtest.h>
#include "../src/Config/Config.h"
#include "../src/Config/ConfigExceptions.h"
#include "../src/Server/ServerExceptions.h"

class ServerTest : public ::testing::Test, public Server {

    ServerTest() : Server(<#initializer#>) {}
};
