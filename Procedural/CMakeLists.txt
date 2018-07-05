cmake_minimum_required(VERSION 3.9)
project(Procedural)

set(CMAKE_CXX_STANDARD 14)

add_library(Procedural ProceduralSpawner.cpp ProceduralSpawner.h ProceduralSpawnerItem.cpp ProceduralSpawnerItem.h ProceduralValueSource.cpp ProceduralValueSource.h)

target_include_directories(Procedural PUBLIC "..")

target_link_libraries(Procedural Core Foundation Scene Scene2D Scene3D )
install (TARGETS Procedural RUNTIME DESTINATION bin)