cmake_minimum_required(VERSION 3.9)
project(UI)

set(CMAKE_CXX_STANDARD 14)

add_library(UI Widget.cpp Widget.h Font.cpp Font.h Label.cpp Label.h UIModule.cpp UIModule.h)

target_include_directories(UI PUBLIC "..")

target_link_libraries(UI Core Foundation Rendering Scene)

INSTALL(FILES Shaders/font.vs Shaders/font.ps Shaders/font.var DESTINATION bin/Shaders)