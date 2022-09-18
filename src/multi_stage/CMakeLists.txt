cmake_minimum_required(VERSION 3.18) 

include_directories(${PROJECT_SOURCE_DIR}/src)

target_sources(${EXECUTABLE} PRIVATE PUBLIC MultiStage.h MultiStage.cpp)

