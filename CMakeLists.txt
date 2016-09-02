cmake_minimum_required(VERSION 3.6)
project(Games)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES hw01.cpp)
add_executable(Games ${SOURCE_FILES})
