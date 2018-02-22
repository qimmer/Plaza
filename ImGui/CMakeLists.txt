cmake_minimum_required(VERSION 3.9)
project(ImGui)

add_library(ImGui imgui/imgui.cpp imgui/imgui_draw.cpp ImGuiRenderer.cpp ImGuiRenderer.h ImGuiModule.cpp ImGuiModule.h ImGuiControls.h ImGuiControls.cpp)

target_include_directories(ImGui PUBLIC ".." ".")
target_link_libraries(ImGui Core Foundation Rendering File)
