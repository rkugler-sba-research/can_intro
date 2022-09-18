cmake_minimum_required(VERSION 3.18)

# set(CMAKE_CXX_COMPILER "/usr/local/bin/afl-g++")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "DEBUG")


set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(can_intro)
include_directories(${PROJECT_SOURCE_DIR}/src)

set(EXECUTABLE can_intro)

add_executable(${EXECUTABLE})

set_property(TARGET ${EXECUTABLE} PROPERTY CXX_STANDARD 14) 

add_subdirectory(src)
