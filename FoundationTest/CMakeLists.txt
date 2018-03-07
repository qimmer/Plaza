cmake_minimum_required(VERSION 3.9)
project(FoundationTest)

set(CMAKE_CXX_STANDARD 14)

add_executable(FoundationTest main.cpp Common.h TestFile.h)

target_link_libraries(FoundationTest Core Foundation Test File)
