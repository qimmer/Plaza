cmake_minimum_required(VERSION 3.9)
project(Plaza)

set(CMAKE_CXX_STANDARD 14)
if(NOT DEFINED BUILD_SHARED_LIBS)
set(BUILD_SHARED_LIBS ON)
endif()

if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/.install)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Plaza Libraries
add_subdirectory(Core)
add_subdirectory(Foundation)
add_subdirectory(File)
add_subdirectory(Networking)
add_subdirectory(Rest)
add_subdirectory(Test)
add_subdirectory(Input)
add_subdirectory(Rendering)
add_subdirectory(Scene)
add_subdirectory(Scene2D)
add_subdirectory(Scene3D)
add_subdirectory(ImGui)
add_subdirectory(UI)
add_subdirectory(Player)
add_subdirectory(Editor)
add_subdirectory(Debug)
add_subdirectory(DataModel)
add_subdirectory(Procedural)
#add_subdirectory(Zip)
add_subdirectory(SQL)
add_subdirectory(Lua)
add_subdirectory(JavaScript)
#add_subdirectory(Xml)
add_subdirectory(Json)
add_subdirectory(StbDecoders)
#add_subdirectory(Excel)
add_subdirectory(Tests)


# Plaza Plugins
add_subdirectory(BgfxRendering)

# Projects