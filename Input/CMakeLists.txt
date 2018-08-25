cmake_minimum_required(VERSION 3.9)
project(Input)

set(CMAKE_CXX_STANDARD 14)

add_library(Input InputModule.cpp InputModule.h Key.cpp Key.h InputContext.cpp InputContext.h)

target_include_directories(Input PUBLIC "..")

target_link_libraries(Input PUBLIC Foundation Core)

if(BUILD_SHARED_LIBS)
install (TARGETS Input RUNTIME DESTINATION bin)
    endif()