cmake_minimum_required(VERSION 3.9)
project(TestJavaScript)

set(CMAKE_CXX_STANDARD 14)

add_library(TestJavaScript SHARED TestJavaScript.cpp TestJavaScript.h)

target_link_libraries(TestJavaScript Core Foundation File Json JavaScript Test)