cmake_minimum_required (VERSION 2.6)

project(FLC)
set(CMAKE_CURRENT_SOURCE_DIR)
include_directories(${FLC_SOURCE_DIR})
add_executable(flc main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 


