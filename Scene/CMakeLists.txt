cmake_minimum_required(VERSION 3.9)
project(Scene)

set(CMAKE_CXX_STANDARD 14)

add_library(Scene Transform.cpp Transform.h Transform2D.h Transform2D.cpp Transform3D.h Transform3D.cpp Camera.cpp Camera.h Model.cpp Model.h SceneModule.cpp SceneModule.h Frustum.h Frustum.cpp Light.h Light.cpp PerspectiveFrustum.cpp PerspectiveFrustum.h OrthographicFrustum.cpp OrthographicFrustum.h Scene.cpp Scene.h)

find_package(OpenMP)
if (OPENMP_FOUND)
    target_link_libraries(Scene "${OpenMP_CXX_FLAGS}")
    target_compile_options(Scene PRIVATE "${OpenMP_CXX_FLAGS}")
endif()

target_include_directories(Scene PUBLIC ".." ".")

target_link_libraries(Scene Core Foundation)
if(BUILD_SHARED_LIBS)
    install (TARGETS Scene RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()