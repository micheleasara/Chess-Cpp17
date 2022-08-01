cmake_minimum_required(VERSION 3.22)
project(Driver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CHESSCPP_PATH ../ChessCpp)
set(CHESSCPP_BUILD_PATH lib)
option(TESTS "Build tests" ON)

add_subdirectory(${CHESSCPP_PATH} ${CHESSCPP_BUILD_PATH})

add_executable(driver ChessCppDriver.cpp)

target_include_directories(driver PUBLIC
                          ${PROJECT_SOURCE_DIR}
                          ${CHESSCPP_PATH}/src/)

target_link_libraries(driver PUBLIC ChessCpp)