cmake_minimum_required(VERSION 3.9)
project(TestGame)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestGame main.cpp)

configure_file(${PROJECT_SOURCE_DIR}/../ImGui/Shaders/imgui.ps ${CMAKE_CURRENT_BINARY_DIR}/Shaders/imgui.ps COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../ImGui/Shaders/imgui.vs ${CMAKE_CURRENT_BINARY_DIR}/Shaders/imgui.vs COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../ImGui/Shaders/imgui.var ${CMAKE_CURRENT_BINARY_DIR}/Shaders/imgui.var COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../Scene2D/Shaders/sprite.ps ${CMAKE_CURRENT_BINARY_DIR}/Shaders/sprite.ps COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../Scene2D/Shaders/sprite.vs ${CMAKE_CURRENT_BINARY_DIR}/Shaders/sprite.vs COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../Scene2D/Shaders/sprite.var ${CMAKE_CURRENT_BINARY_DIR}/Shaders/sprite.var COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../Bgfx/shaders/bgfx_shader.sh ${CMAKE_CURRENT_BINARY_DIR}/Shaders/bgfx_shader.sh COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../Bgfx/shaders/common.sh ${CMAKE_CURRENT_BINARY_DIR}/Shaders/common.sh COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../Bgfx/shaders/shaderlib.sh ${CMAKE_CURRENT_BINARY_DIR}/Shaders/shaderlib.sh COPYONLY)

target_link_libraries(TestGame Core Foundation Rendering Bgfx Editor ImGui Player Scene2D)
