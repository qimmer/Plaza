cmake_minimum_required(VERSION 3.9)
project(Rest)

set(CMAKE_CXX_STANDARD 14)

add_library(Rest RestModule.cpp RestModule.h RestEntityRouting.cpp RestEntityRouting.h RestResourceRouting.cpp RestResourceRouting.h)

if(MSVC)
    target_sources(Rest PRIVATE Rest.def)
endif()

target_link_libraries(Rest PRIVATE Core Foundation Json Networking)
target_include_directories(Rest PUBLIC "..")
