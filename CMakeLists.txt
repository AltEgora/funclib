cmake_minimum_required(VERSION 3.24)
project(funclib CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(funclib
    src/function.cpp
    src/functions.cpp
    src/factory.cpp
)
target_include_directories(funclib PUBLIC src)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
    tests/test_functions.cpp
    tests/test_factory.cpp
    tests/test_operators.cpp
    tests/test_deriv.cpp
)
target_link_libraries(tests funclib GTest::gtest_main)

option(COVERAGE "Build with coverage instrumentation" OFF)
if(COVERAGE)
    target_compile_options(funclib PRIVATE --coverage -O0)
    target_compile_options(tests PRIVATE --coverage -O0)
    target_link_options(tests PRIVATE --coverage)
endif()

include(GoogleTest)
gtest_discover_tests(tests PROPERTIES TIMEOUT 10)
