cmake_minimum_required(VERSION 3.23.2)
project(webserv)
project(webserv_lib)

add_compile_options(-gdwarf-4 -O0 -fno-limit-debug-info  -Wall -Wextra -Werror)
set(CMAKE_CXX_STANDARD 98)

add_executable(webserv src/main.cpp
        src/utils/Utils.cpp
        src/utils/StringUtils.cpp
        src/utils/FSUtils.cpp
        src/Server/server/response/ResponseStaticUtils.cpp
        src/Server/server/response/ServerResponse.cpp
        src/Server/server/CheckUpload.cpp
        src/Server/server/ServerExceptions.cpp
        src/Server/server/SynthFoundExact.cpp
        src/Server/server/Server.cpp
        src/Server/server/LocationSynth.cpp
        src/Server/server/PerformUpload.cpp
        src/Server/ServerManagerHandleEvents.cpp
        src/Server/ServerManagerInit.cpp
        src/Server/connection/Connection.cpp
        src/Server/connection/request/ClientRequest.cpp
        src/Server/connection/request/ClientRequestURLHandlers.cpp
        src/Server/connection/request/ClientRequestBodyProcessing.cpp
        src/Server/connection/request/ClientRequestRequestHandlers.cpp
        src/Server/connection/request/ClientRequestURLParamsHandlers.cpp
        src/Server/connection/request/RequestExceptions.cpp
        src/Server/ServerManagerRun.cpp
        src/Server/ServerManagerUtils.cpp
        src/Server/ServerManager.cpp
        src/Config/config/ConfigCreate.cpp
        src/Config/config/Config.cpp
        src/Config/config/ConfigParse.cpp
        src/Config/Node.cpp
        src/Config/server_configuration/ServerConfiguration.cpp
        src/Config/server_configuration/ServerConfigurationLocationSearch.cpp
        src/Config/server_configuration/ServerConfigurationLocationHandler.cpp
        src/Config/location/ErrPage.cpp
        src/Config/location/Location.cpp
        src/Config/location/LocationHandleReturn.cpp
        src/Config/location/LimitExcept.cpp
        src/Config/server_configuration/Host.cpp
        src/Server/ServerManagerCGI.cpp
        src/Server/server/ServerCGIHandler.cpp
)
