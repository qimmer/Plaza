cmake_minimum_required(VERSION 3.9)
project(TestGame)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestGame main.cpp)

target_link_libraries(TestGame Core Foundation Rendering BgfxRendering Editor ImGui Player Scene2D Scene3D Json StbDecoders)
install (TARGETS TestGame RUNTIME DESTINATION bin)
INSTALL(FILES game.json DESTINATION bin)