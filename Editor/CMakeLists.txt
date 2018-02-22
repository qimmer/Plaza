cmake_minimum_required(VERSION 3.9)
project(Editor)

set(CMAKE_CXX_STANDARD 14)

add_library(Editor MainMenu.cpp MainMenu.h EditorModule.cpp EditorModule.h FileExplorer.cpp FileExplorer.h FileEditor.cpp FileEditor.h CodeEditor.cpp CodeEditor.h LuaEditor.cpp LuaEditor.h EntityExplorer.cpp EntityExplorer.h Selection.cpp Selection.h PropertyEditor.cpp PropertyEditor.h Project.h Project.cpp ProjectReference.h ProjectReference.cpp)

target_include_directories(Editor PUBLIC "..")

target_link_libraries(Editor Core Foundation Rendering ImGui)
