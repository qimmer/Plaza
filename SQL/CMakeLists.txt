cmake_minimum_required(VERSION 3.9)
project(SQL)

set(CMAKE_CXX_STANDARD 14)

add_library(SQL SQLPersistancePoint.h SQLPersistancePoint.cpp SQLModule.cpp SQLModule.h)

target_include_directories(SQL PUBLIC ".." ".")

target_link_libraries(SQL Core Foundation)
