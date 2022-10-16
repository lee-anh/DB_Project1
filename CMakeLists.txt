cmake_minimum_required(VERSION 3.14)
project(p1)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  database_test
  database_test.cpp
  Database.cpp
)
target_link_libraries(
  database_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(database_test)