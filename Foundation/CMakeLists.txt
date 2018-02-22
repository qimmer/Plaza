cmake_minimum_required(VERSION 3.9)
project(Foundation)

set(CMAKE_CXX_STANDARD 14)

add_library(Foundation AppLoop.cpp AppLoop.h Visibility.cpp Visibility.h FoundationModule.cpp FoundationModule.h Stream.cpp Stream.h MemoryStream.cpp MemoryStream.h Invalidation.cpp Invalidation.h Persistance.cpp Persistance.h PersistancePoint.cpp PersistancePoint.h StreamedPersistancePoint.cpp StreamedPersistancePoint.h)

target_include_directories(Foundation PUBLIC ".." ".")

target_link_libraries(Foundation Core)
