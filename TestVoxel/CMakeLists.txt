cmake_minimum_required(VERSION 3.9)
project(TestVoxel)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestVoxel main.cpp)

target_link_libraries(TestVoxel Core Foundation Rendering BgfxRendering Editor ImGui Player Scene2D Scene3D)
