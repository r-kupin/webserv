cmake_minimum_required(VERSION 3.24)
project(webserv)
project(webserv_lib)

add_compile_options(-gdwarf-4 -O1 -fno-limit-debug-info  -Wall -Wextra -Werror)
set(CMAKE_CXX_STANDARD 98)

add_executable(webserv src/main.cpp
        src/Config/Config.cpp
        src/Config/Config.h
        src/Server/Server.cpp
        src/Server/Server.h
        src/Config/ConfigExceptions.h
        src/Config/ConfigParser.cpp
        src/Config/ChonfigProcessingUtils.cpp
        src/Config/ConfigChecker.cpp
        src/Server/ServerManager.cpp
        src/Server/ServerManager.h
        src/Config/ConfigLocationChecker.cpp
        src/Config/ConfigSubmodules.h
        src/Config/ConfigSubmodules.cpp
        src/Server/ServerExceptions.h
        src/Server/ClientRequest.cpp
        src/Server/ClientRequest.h
        src/Server/ClientRequest.cpp
        src/Config/location/Location.h
        src/Config/location/Location.h
        src/Config/location/ErrPage.h
        src/Config/location/LimitExcept.h
        src/Config/location/Location.cpp
        src/Config/location/ErrPage.cpp
        src/Server/ServerResponse.cpp
        src/Server/ServerResponse.h
        src/Config/location/LocationLimitExcept.cpp
        src/Config/location/LimitExcept.cpp)
add_library(webserv_lib
        src/Config/Config.cpp
        src/Config/Config.h
        src/Server/Server.cpp
        src/Server/Server.h
        src/Config/ConfigExceptions.h
        src/Config/ConfigParser.cpp
        src/Config/ChonfigProcessingUtils.cpp
        src/Config/ConfigChecker.cpp
        src/Server/ServerManager.cpp
        src/Server/ServerManager.h
        src/Config/ConfigLocationChecker.cpp
        src/Config/ConfigSubmodules.h
        src/Config/ConfigSubmodules.cpp
        src/Server/ServerExceptions.h
        src/Server/ClientRequest.cpp
        src/Server/ClientRequest.h
        src/Server/ClientRequest.cpp
        src/Config/location/Location.h
        src/Config/location/Location.h
        src/Config/location/ErrPage.h
        src/Config/location/LimitExcept.h
        src/Config/location/Location.cpp
        src/Config/location/ErrPage.cpp
        src/Server/ServerResponse.cpp
        src/Server/ServerResponse.h
        src/Config/location/LocationLimitExcept.cpp
        src/Config/location/LimitExcept.cpp)

add_subdirectory(test)