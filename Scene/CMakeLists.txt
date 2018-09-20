cmake_minimum_required(VERSION 3.9)
project(Scene)

set(CMAKE_CXX_STANDARD 14)

add_library(Scene Transform.cpp Transform.h Camera.cpp Camera.h Model.cpp Model.h Scene.cpp Scene.h SceneNode.cpp SceneNode.h SceneModule.cpp SceneModule.h Frustum.h Frustum.cpp Light.h Light.cpp PerspectiveFrustum.cpp PerspectiveFrustum.h OrthographicFrustum.cpp OrthographicFrustum.h SceneRenderer.h SceneRenderer.cpp)

target_include_directories(Scene PUBLIC ".." ".")

target_link_libraries(Scene Core Rendering)
if(BUILD_SHARED_LIBS)
    install (TARGETS Scene RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()