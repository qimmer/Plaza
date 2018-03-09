cmake_minimum_required(VERSION 3.9)
project(Foundation)

set(CMAKE_CXX_STANDARD 14)

add_library(Foundation AppLoop.cpp AppLoop.h Visibility.cpp Visibility.h FoundationModule.cpp FoundationModule.h Stream.cpp Stream.h MemoryStream.cpp MemoryStream.h Persistance.cpp Persistance.h PersistancePoint.cpp PersistancePoint.h PropertyBinding.cpp PropertyBinding.h VirtualPath.cpp VirtualPath.h)

target_include_directories(Foundation PUBLIC ".." ".")

target_link_libraries(Foundation Core)
