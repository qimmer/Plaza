cmake_minimum_required(VERSION 3.9)
project(Editor)

set(CMAKE_CXX_STANDARD 14)

add_library(Editor MainMenu.cpp MainMenu.h EditorModule.cpp EditorModule.h ToolWindows/FileExplorer.cpp ToolWindows/FileExplorer.h Editors/CodeEditor.cpp Editors/CodeEditor.h ToolWindows/EntityExplorer.cpp ToolWindows/EntityExplorer.h Selection.cpp Selection.h PropertyEditor.cpp PropertyEditor.h Project.h Project.cpp ProjectReference.h ProjectReference.cpp EditorStyle.cpp EditorStyle.h Editors/SceneEditor.cpp Editors/SceneEditor.h Tools/FirstPersonCamera.cpp Tools/FirstPersonCamera.h EditorStandardCommands.cpp EditorStandardCommands.h EditorSettings.cpp EditorSettings.h Editors/Texture2DViewer.cpp Editors/Texture2DViewer.h)

target_include_directories(Editor PUBLIC "..")

target_link_libraries(Editor Core Foundation Rendering ImGui Scene Scene3D)

INSTALL(FILES Fonts/fontawesome-webfont.ttf Fonts/MaterialIcons-Regular.ttf Fonts/segoeui.ttf Fonts/DroidSans.ttf Fonts/DroidSans-Bold.ttf DESTINATION bin/Fonts)