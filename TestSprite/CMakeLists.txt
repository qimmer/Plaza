cmake_minimum_required(VERSION 3.9)
project(TestSprite)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestSprite main.cpp)

target_link_libraries(TestSprite Core Foundation Rendering BgfxRendering Editor ImGui Player Scene2D)
