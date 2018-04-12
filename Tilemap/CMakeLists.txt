cmake_minimum_required(VERSION 3.9)
project(Tilemap)

set(CMAKE_CXX_STANDARD 14)

add_library(Tilemap MeshInstance.cpp MeshInstance.h Scene.cpp Scene.h SceneNode.cpp SceneNode.h SceneModule.cpp SceneModule.h)

target_include_directories(Tilemap PUBLIC "..")

target_link_libraries(Tilemap Core Foundation Rendering)
