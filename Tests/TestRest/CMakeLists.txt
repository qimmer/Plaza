cmake_minimum_required(VERSION 3.9)
project(TestRest)

set(CMAKE_CXX_STANDARD 14)

add_library(TestRest SHARED TestRest.cpp TestRest.h)

target_link_libraries(TestRest Core Foundation File Json Networking Rest Test)