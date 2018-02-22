cmake_minimum_required(VERSION 3.9)
project(File)

set(CMAKE_CXX_STANDARD 14)

add_library(File FileStream.cpp FileStream.h Folder.cpp Folder.h FileModule.cpp FileModule.h)

target_include_directories(File PUBLIC ".." ".")

target_link_libraries(File Foundation Core)
