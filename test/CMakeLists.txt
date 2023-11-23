# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with lib
add_executable(Google_Tests_run ConfigParsingTest.cpp ../src/Config/ConfigChecker.cpp ConfigProcessingTest.cpp LocationNodeTest.cpp LocationTest.cpp ServerConfigTest.cpp LimitExceptNodeTest.cpp MainNodeTest.cpp ServerNodeTest.cpp CheckServerTest.cpp)
target_link_libraries(Google_Tests_run webserv_lib)
target_link_libraries(Google_Tests_run gtest gtest_main)