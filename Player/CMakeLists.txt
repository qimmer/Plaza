cmake_minimum_required(VERSION 3.9)
project(Player)

set(CMAKE_CXX_STANDARD 14)

add_library(Player PlayerModule.cpp PlayerModule.h PlayerContext.cpp PlayerContext.h)

target_include_directories(Player PUBLIC "..")

target_link_libraries(Player Core Foundation Rendering File)
install (TARGETS Player RUNTIME DESTINATION bin)