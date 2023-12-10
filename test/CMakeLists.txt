# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with lib
add_executable(Google_Tests_run
        tests/server_test/setup_test/ServerConfigTest.cpp
        tests/server_test/functionality_test/ServerTest.cpp
        tests/config_test/location_class_test/setup_test/HandleReturnTest.cpp
        tests/config_test/location_class_test/setup_test/HandleAddErrPagesTest.cpp
        tests/config_test/location_class_test/setup_test/HandleUpdateIndexTest.cpp
        tests/config_test/location_class_test/setup_test/LocationMarkDirectiveTest.cpp
        tests/config_test/location_class_test/setup_test/HandleLimitExceptTest.cpp
        tests/config_test/location_class_test/setup_test/LocationProcessDirectivesTest.cpp
        tests/config_test/location_class_test/setup_test/LocationConstructorTest.cpp
        tests/config_test/location_class_test/functionality_test/LocationFindSublocationTest.cpp
        tests/config_test/config_class_test/ConfigServerSubcontextsHandlingTest.cpp
        tests/config_test/config_class_test/ConfigMainContextCheck.cpp
        tests/config_test/config_class_test/ConfigFileCheckTest.cpp
        tests/config_test/server_configuration_class_test/setup_test/HandleDirectivesTest.cpp
        tests/config_test/server_configuration_class_test/setup_test/HandleLocationTest.cpp
        tests/config_test/server_configuration_class_test/functionality_test/CheckCorrectLocationContextTest.cpp
)
target_link_libraries(Google_Tests_run webserv_lib)
target_link_libraries(Google_Tests_run gtest gtest_main)