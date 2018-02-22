cmake_minimum_required(VERSION 3.9)
project(Voxel)

set(CMAKE_CXX_STANDARD 14)

add_library(Voxel VoxelMesh.h VoxelTile.h VoxelTileMap.h VoxelMesh.cpp VoxelTile.cpp VoxelTileMap.cpp stb_voxel_render.h)

target_include_directories(Voxel PUBLIC ".." ".")

target_link_libraries(Voxel Core Foundation Rendering Scene3D)
