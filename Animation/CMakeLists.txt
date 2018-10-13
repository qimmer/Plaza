cmake_minimum_required(VERSION 3.9)
project(File)

set(CMAKE_CXX_STANDARD 14)

add_library(Animation Animation.cpp Animation.h AnimationPlayer.cpp AnimationPlayer.h AnimationSet.cpp AnimationSet.h Animator.cpp Animator.h AnimationModule.cpp AnimationModule.h)

target_include_directories(Animation PUBLIC "..")
target_link_libraries(Animation Core Foundation)

if(MSVC)
    target_sources(Animation PRIVATE Animation.def)
endif()

if(BUILD_SHARED_LIBS)
    install (TARGETS Animation RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()