# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with lib
add_executable(Google_Tests_run
        unit_tests/utils_test/UtilsTest.cpp
        unit_tests/server_test/request_test/RequestLineLevelTest.cpp
        unit_tests/server_test/request_test/URLLevelTest.cpp
        unit_tests/server_test/request_test/SocketLevelTest.cpp
        unit_tests/server_test/request_test/ParsedRequestLevelTest.cpp
        unit_tests/server_test/functionality_test/SynthIndexTest.cpp
        unit_tests/server_test/functionality_test/FindIndexTest.cpp
        unit_tests/server_test/functionality_test/LocationSynthUtilsTest.cpp
        unit_tests/server_test/functionality_test/LocationSynthTest_SimpleIndex.cpp
        unit_tests/server_test/functionality_test/SynthFileTest.cpp
        unit_tests/config_test/location_class_test/setup_test/HandleReturnTest.cpp
        unit_tests/config_test/location_class_test/setup_test/HandleAddErrPagesTest.cpp
        unit_tests/config_test/location_class_test/setup_test/HandleUpdateIndexTest.cpp
        unit_tests/config_test/location_class_test/setup_test/LocationStaticToolsTest.cpp
        unit_tests/config_test/location_class_test/setup_test/LocationMarkDirectiveTest.cpp
        unit_tests/config_test/location_class_test/setup_test/HandleLimitExceptTest.cpp
        unit_tests/config_test/location_class_test/setup_test/LocationProcessDirectivesTest.cpp
        unit_tests/config_test/location_class_test/functionality_test/LocationFindSublocationTest.cpp
        unit_tests/config_test/config_class_test/setup_from_file_config/ConfigSetupFromFileTest.cpp
        unit_tests/config_test/config_class_test/setup_from_file_config/ConfigFileCheckTest.cpp
        unit_tests/config_test/config_class_test/setup_from_nodes/ConfigServerSubcontextsHandlingTest.cpp
        unit_tests/config_test/config_class_test/setup_from_nodes/ConfigMainContextCheck.cpp
        unit_tests/config_test/server_configuration_class_test/setup_test/HandleDirectivesTest.cpp
        unit_tests/config_test/server_configuration_class_test/setup_test/HandleMultistepLocation.cpp
        unit_tests/config_test/server_configuration_class_test/setup_test/HandleSingleStepLocation.cpp
        unit_tests/config_test/server_configuration_class_test/functionality_test/NestedConfigLocationSearchTest.cpp
        unit_tests/config_test/server_configuration_class_test/functionality_test/CheckCorrectLocationContextTest.cpp
        unit_tests/config_test/server_configuration_class_test/functionality_test/SimpleConfigLocationSearchTest.cpp
        unit_tests/config_test/config_class_test/setup_from_file_config/ParsingToolsTest.cpp
        unit_tests/server_test/response_test/SimpleResponseTest.cpp
        unit_tests/server_test/response_test/SimpleIndexConfigResponseTest.cpp
)

target_link_libraries(Google_Tests_run webserv_lib)
target_link_libraries(Google_Tests_run gtest gtest_main)