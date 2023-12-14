# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with lib
add_executable(Google_Tests_run
        tests/config_file_test/ConfigParsingTest.cpp
        tests/node_test/LimitExceptContextTest.cpp
        tests/node_test/LocationNodeTest.cpp
        tests/node_test/LimitExceptNodeTest.cpp
        tests/node_test/MainNodeTest.cpp
        tests/node_test/ServerNodeTest.cpp
        tests/node_test/LocationContextTest.cpp
        tests/server_test/setup_test/ServerConfigTest.cpp
        tests/server_test/functionality_test/ServerTest.cpp
        tests/location_class_test/setup_test/LocationMarkDirectiveTest.cpp
        tests/location_class_test/setup_test/LocationConstructorTest.cpp
        tests/location_class_test/setup_test/LocationProcessDirectivesTest.cpp
        tests/location_class_test/functionality_test/LocationFindSublocationTest.cpp
        tests/location_class_test/setup_test/HandleUpdateIndexTest.cpp
        tests/location_class_test/setup_test/HandleReturnTest.cpp
        tests/location_class_test/setup_test/HandleAddErrPagesTest.cpp)

target_link_libraries(Google_Tests_run webserv_lib)
target_link_libraries(Google_Tests_run gtest gtest_main)