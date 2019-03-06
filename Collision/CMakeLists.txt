cmake_minimum_required(VERSION 3.9)
project(Collision)

set(CMAKE_CXX_STANDARD 14)

add_library(Collision CollisionModule.h CollisionModule.cpp)

target_include_directories(Collision PUBLIC "..")
if(MSVC)
    target_sources(Collision PRIVATE Collision.def)
endif()
target_link_libraries(Collision Core Foundation Scene)
