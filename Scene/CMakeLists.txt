cmake_minimum_required(VERSION 3.9)
project(Scene)

set(CMAKE_CXX_STANDARD 14)

add_library(Scene Transform.cpp Transform.h Camera.cpp Camera.h MeshInstance.cpp MeshInstance.h Scene.cpp Scene.h SceneNode.cpp SceneNode.h SimpleSceneRenderer.h SimpleSceneRenderer.cpp SceneModule.cpp SceneModule.h ScenePicker.cpp ScenePicker.h MousePicker.cpp MousePicker.h)

target_include_directories(Scene PUBLIC ".." ".")

target_link_libraries(Scene Core Rendering)
if(BUILD_SHARED_LIBS)
    install (TARGETS Scene RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()