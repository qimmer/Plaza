cmake_minimum_required(VERSION 3.9)
project(Editor)

set(CMAKE_CXX_STANDARD 14)

add_library(Editor Editor.cpp Editor.h)

target_include_directories(Editor PUBLIC ".." ".")
target_link_libraries(Editor Foundation Core Gui Rendering Widgets)

if(MSVC)
    target_sources(Editor PRIVATE Editor.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Editor RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()