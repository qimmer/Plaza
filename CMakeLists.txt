cmake_minimum_required(VERSION 3.9)
project(Plaza)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/.install)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Plaza Libraries
add_subdirectory(Core)
add_subdirectory(Foundation)
add_subdirectory(File)
add_subdirectory(Test)
add_subdirectory(Logic)
add_subdirectory(Input)
add_subdirectory(Rendering)
add_subdirectory(Scene)
add_subdirectory(Scene2D)
add_subdirectory(Scene3D)
add_subdirectory(ImGui)
add_subdirectory(UI)
add_subdirectory(Player)
add_subdirectory(Editor)
add_subdirectory(DataModel)
add_subdirectory(Procedural)
#add_subdirectory(Zip)
add_subdirectory(SQL)
add_subdirectory(Lua)
#add_subdirectory(Xml)
add_subdirectory(Json)
add_subdirectory(StbDecoders)
#add_subdirectory(Excel)


# Plaza Plugins
add_subdirectory(BgfxRendering)

# Tests
add_subdirectory(CoreTest)
add_subdirectory(FoundationTest)
add_subdirectory(RenderingTest)

# Projects
add_subdirectory(TestGame)
add_subdirectory(TestVoxel)
add_subdirectory(TestSprite)