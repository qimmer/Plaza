cmake_minimum_required(VERSION 3.9)
project(Input)

set(CMAKE_CXX_STANDARD 14)

add_library(Input InputState.cpp InputState.h InputModule.cpp InputModule.h Key.cpp Key.h)

target_include_directories(Input PUBLIC "..")

target_link_libraries(Input PUBLIC Foundation Core)
install (TARGETS Input RUNTIME DESTINATION bin)