cmake_minimum_required(VERSION 3.9)
project(Scene3D)

set(CMAKE_CXX_STANDARD 14)

add_library(Scene3D Transform3D.cpp Transform3D.h Camera3D.cpp Camera3D.h Scene3DModule.cpp Scene3DModule.h Voxel/VoxelPalette.cpp Voxel/VoxelPalette.h Voxel/VoxelColor.cpp Voxel/VoxelColor.h Voxel/VoxelMesh.cpp Voxel/VoxelMesh.h Voxel/VoxelChunk.cpp Voxel/VoxelChunk.h Voxel/VoxPersistance.cpp Voxel/VoxPersistance.h Voxel/VoxelChunkSpawner.cpp Voxel/VoxelChunkSpawner.h)

target_include_directories(Scene3D PUBLIC ".." ".")

target_link_libraries(Scene3D Core Rendering Scene)

INSTALL(DIRECTORY Assets DESTINATION bin)
install (TARGETS Scene3D RUNTIME DESTINATION bin)