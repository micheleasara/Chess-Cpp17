cmake_minimum_required(VERSION 3.22)
project(ChessCppBuild)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
option(TESTS "Build tests" OFF)

add_subdirectory(src)
include_directories(src)

if(TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()