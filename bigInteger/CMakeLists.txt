cmake_minimum_required(VERSION 3.14)
project(biginteger)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
include_directories(${CPR_INCLUDE_DIRS})

enable_testing()

add_executable(
    test_biginteger
    test_biginteger.cc
)
target_link_libraries(
    test_biginteger
    gtest_main
)

add_executable(
    test_rational
    test_rational.cc
)
target_link_libraries(
    test_rational
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_biginteger)
gtest_discover_tests(test_rational)