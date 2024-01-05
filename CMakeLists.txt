cmake_minimum_required(VERSION 3.23.2)
project(webserv)
project(webserv_lib)

add_compile_options(-gdwarf-4 -O1 -fno-limit-debug-info  -Wall -Wextra -Werror)
set(CMAKE_CXX_STANDARD 98)

add_executable(webserv src/main.cpp
        src/Config/config/Config.cpp
        src/Server/Server.cpp
        src/Config/config/ConfigParse.cpp
        src/Config/config/ConfigCreate.cpp
        src/Server/ServerManager.cpp
        src/Config/server_configuration/ServerConfiguration.cpp
        src/Server/request/ClientRequest.cpp
        src/Config/location/Location.cpp
        src/Config/location/ErrPage.cpp
        src/Server/response/ServerResponse.cpp
        src/Config/location/LimitExcept.cpp
        src/Config/server_configuration/ServerConfigurationLocationHandler.cpp
        src/Config/Node.cpp
        src/Config/location/LocationHandleReturn.cpp
        src/Server/ServerInit.cpp
        src/Server/ServerExceptions.cpp
        src/Server/request/RequestExceptions.cpp
        src/Server/request/ClientRequestURLHandlers.cpp
        src/Server/request/ClientRequestRequestHandlers.cpp
        src/Server/request/ClientRequestURLParamsHandlers.cpp
        src/Config/server_configuration/ServerConfigurationLocationSearch.cpp
        src/Server/location_synth/SynthFoundExact.cpp
        src/Server/location_synth/SynthNotFound.cpp
        src/Server/location_synth/LocationSynth.cpp
        src/Server/response/ResponseStaticUtils.cpp
        src/utils/Utils.cpp
)
add_library(webserv_lib
        src/Config/config/Config.cpp
        src/Server/Server.cpp
        src/Config/config/ConfigParse.cpp
        src/Config/config/ConfigCreate.cpp
        src/Server/ServerManager.cpp
        src/Config/server_configuration/ServerConfiguration.cpp
        src/Server/request/ClientRequest.cpp
        src/Config/location/Location.cpp
        src/Config/location/ErrPage.cpp
        src/Server/response/ServerResponse.cpp
        src/Config/location/LimitExcept.cpp
        src/Config/server_configuration/ServerConfigurationLocationHandler.cpp
        src/Config/Node.cpp
        src/Config/location/LocationHandleReturn.cpp
        src/Server/ServerInit.cpp
        src/Server/ServerExceptions.cpp
        src/Server/request/RequestExceptions.cpp
        src/Server/request/ClientRequestURLHandlers.cpp
        src/Server/request/ClientRequestRequestHandlers.cpp
        src/Server/request/ClientRequestURLParamsHandlers.cpp
        src/Config/server_configuration/ServerConfigurationLocationSearch.cpp
        src/Server/location_synth/SynthFoundExact.cpp
        src/Server/location_synth/SynthNotFound.cpp
        src/Server/location_synth/LocationSynth.cpp
        src/Server/response/ResponseStaticUtils.cpp
        src/utils/Utils.cpp
)

add_subdirectory(test)