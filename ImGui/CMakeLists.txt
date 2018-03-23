cmake_minimum_required(VERSION 3.9)
project(ImGui)

add_library(ImGui imgui/imgui.cpp imgui/imgui_draw.cpp ImGuiRenderer.cpp ImGuiRenderer.h ImGuiModule.cpp ImGuiModule.h ImGuiControls.h ImGuiControls.cpp)

target_include_directories(ImGui PUBLIC ".." "." "./imgui/addons" "./imgui")
target_link_libraries(ImGui Core Foundation Rendering File)
target_compile_definitions(ImGui PUBLIC DIMGUI_INCLUDE_IMGUI_USER_H)

INSTALL(FILES Shaders/imgui.vs Shaders/imgui.ps Shaders/imgui.var DESTINATION bin/Shaders)