cmake_minimum_required(VERSION 3.9)
project(UI)

set(CMAKE_CXX_STANDARD 14)

add_library(UI Widget.cpp Widget.h Font.cpp Font.h Label.cpp Label.h UIModule.cpp UIModule.h UICamera.cpp UICamera.h Style.cpp Style.h ../SVG/Path.cpp ../SVG/Path.h ../SVG/PathSegment.cpp ../SVG/PathSegment.h ../SVG/RectSegment.cpp ../SVG/RectSegment.h ../SVG/TextSegment.cpp ../SVG/TextSegment.h)

target_include_directories(UI PUBLIC "..")

target_link_libraries(UI Core Foundation Rendering Scene Scene2D)

INSTALL(DIRECTORY Assets DESTINATION bin)

if(BUILD_SHARED_LIBS)
install (TARGETS UI RUNTIME DESTINATION bin)
    endif()