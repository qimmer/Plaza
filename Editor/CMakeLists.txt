cmake_minimum_required(VERSION 3.9)
project(Editor)

set(CMAKE_CXX_STANDARD 14)

add_library(Editor MainMenu.cpp MainMenu.h EditorModule.cpp EditorModule.h Editors/CodeEditor.cpp Editors/CodeEditor.h ToolWindows/EntityExplorer.cpp ToolWindows/EntityExplorer.h Selection.cpp Selection.h ToolWindows/PropertyEditor.cpp ToolWindows/PropertyEditor.h EditorStyle.cpp EditorStyle.h Editors/SceneEditor.cpp Editors/SceneEditor.h Tools/FirstPersonCameraTool.cpp Tools/FirstPersonCameraTool.h EditorStandardCommands.cpp EditorStandardCommands.h Editors/Texture2DViewer.cpp Editors/Texture2DViewer.h EditorContext.cpp EditorContext.h EditorView.cpp EditorView.h MenuItem.cpp MenuItem.h)

target_include_directories(Editor PUBLIC "..")

target_link_libraries(Editor Core Foundation Rendering ImGui Scene Scene3D)

INSTALL(DIRECTORY Assets DESTINATION bin)

if(BUILD_SHARED_LIBS)
install (TARGETS Editor RUNTIME LIBRARY DESTINATION bin)

    endif()