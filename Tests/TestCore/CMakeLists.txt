cmake_minimum_required(VERSION 3.9)
project(TestCore)

set(CMAKE_CXX_STANDARD 14)

add_library(TestCore SHARED TestCore.cpp TestCore.h)

target_link_libraries(TestCore Core Test)