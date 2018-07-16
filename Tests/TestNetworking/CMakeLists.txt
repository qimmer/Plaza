cmake_minimum_required(VERSION 3.9)
project(TestNetworking)

set(CMAKE_CXX_STANDARD 14)

add_library(TestNetworking SHARED TestNetworking.cpp TestNetworking.h)

target_link_libraries(TestNetworking Core Foundation File Json Networking Test)