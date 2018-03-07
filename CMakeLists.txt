cmake_minimum_required(VERSION 3.9)
project(Plaza)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/.install)

# Plaza Libraries
add_subdirectory(Core)
add_subdirectory(Foundation)
add_subdirectory(File)
add_subdirectory(Test)
add_subdirectory(Rendering)
add_subdirectory(Scene)
add_subdirectory(Scene2D)
add_subdirectory(Scene3D)
add_subdirectory(ImGui)
add_subdirectory(Player)
add_subdirectory(Editor)
#add_subdirectory(Zip)
add_subdirectory(SQL)
#add_subdirectory(Xml)
add_subdirectory(Json)
add_subdirectory(StbDecoders)
#add_subdirectory(Excel)
add_subdirectory(Voxel)


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