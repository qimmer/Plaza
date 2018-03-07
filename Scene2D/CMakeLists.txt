cmake_minimum_required(VERSION 3.9)
project(Scene2D)

set(CMAKE_CXX_STANDARD 14)

add_library(Scene2D Transform2D.cpp Transform2D.h Sprite.cpp Sprite.h Camera2D.cpp Camera2D.h Scene2DModule.cpp Scene2DModule.h Tilemap2D.cpp Tilemap2D.h)

target_include_directories(Scene2D PUBLIC ".." ".")

target_link_libraries(Scene2D Core Rendering Scene File)

INSTALL(FILES Shaders/sprite.vs Shaders/sprite.ps Shaders/sprite.var DESTINATION bin/Shaders)