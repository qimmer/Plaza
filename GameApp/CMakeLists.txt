cmake_minimum_required(VERSION 3.9)
project(GameApp)

set(CMAKE_CXX_STANDARD 14)

add_library(GameApp)

target_include_directories(File PUBLIC "..")

target_link_libraries(File Foundation Core)

if(MSVC)
    target_sources(GameApp PRIVATE GameApp.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS GameApp RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()