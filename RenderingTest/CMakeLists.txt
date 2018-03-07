cmake_minimum_required(VERSION 3.9)
project(RenderingTest)

set(CMAKE_CXX_STANDARD 14)

add_executable(RenderingTest main.cpp Common.h TestCube.h)

target_link_libraries(RenderingTest Core Foundation Rendering Test BgfxRendering)
