cmake_minimum_required(VERSION 3.9)
project(Debug)

set(CMAKE_CXX_STANDARD 14)

add_library(Debug )

target_include_directories(Debug PUBLIC "..")

target_link_libraries(Debug Core Foundation Networking Json JavaScript Rest)

install (TARGETS Debug RUNTIME DESTINATION bin)