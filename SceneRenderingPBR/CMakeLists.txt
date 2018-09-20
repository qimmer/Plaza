cmake_minimum_required(VERSION 3.9)
project(SceneRenderingHDR)

set(CMAKE_CXX_STANDARD 14)

add_library(SceneRenderingHDR PBRCamera.h PBRCamera.cpp)

target_include_directories(SceneRenderingHDR PUBLIC "..")

target_link_libraries(SceneRenderingHDR Core Scene Rendering)

if(BUILD_SHARED_LIBS)
    install (TARGETS SceneRenderingHDR RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()