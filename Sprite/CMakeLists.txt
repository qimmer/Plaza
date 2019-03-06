cmake_minimum_required(VERSION 3.9)
project(Sprite)

set(CMAKE_CXX_STANDARD 14)

add_library(Sprite SpriteModule.h SpriteModule.cpp Tilemap.h Tilemap.cpp Sprite.h Sprite.cpp)

target_include_directories(Sprite PUBLIC "..")
if(MSVC)
    target_sources(Sprite PRIVATE Sprite.def)
endif()
target_link_libraries(Sprite Core Foundation Scene Rendering)
