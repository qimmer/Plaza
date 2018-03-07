cmake_minimum_required(VERSION 3.9)
project(TestVoxel)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestVoxel main.cpp)

configure_file(${PROJECT_SOURCE_DIR}/../ImGui/Shaders/imgui.ps ${CMAKE_CURRENT_BINARY_DIR}/Shaders/imgui.ps COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../ImGui/Shaders/imgui.vs ${CMAKE_CURRENT_BINARY_DIR}/Shaders/imgui.vs COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../ImGui/Shaders/imgui.var ${CMAKE_CURRENT_BINARY_DIR}/Shaders/imgui.var COPYONLY)
#configure_file(${PROJECT_SOURCE_DIR}/../Bgfx/shaders/bgfx_shader.sh ${CMAKE_CURRENT_BINARY_DIR}/Shaders/bgfx_shader.sh COPYONLY)
#configure_file(${PROJECT_SOURCE_DIR}/../Bgfx/shaders/common.sh ${CMAKE_CURRENT_BINARY_DIR}/Shaders/common.sh COPYONLY)
#configure_file(${PROJECT_SOURCE_DIR}/../Bgfx/shaders/shaderlib.sh ${CMAKE_CURRENT_BINARY_DIR}/Shaders/shaderlib.sh COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/Textures/minecraft.png ${CMAKE_CURRENT_BINARY_DIR}/Textures/minecraft.png COPYONLY)

target_link_libraries(TestVoxel Core Foundation Rendering BgfxRendering Editor ImGui Player Scene2D Scene3D Voxel)
