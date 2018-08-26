cmake_minimum_required(VERSION 3.9)
project(StbDecoders)

set(CMAKE_CXX_STANDARD 14)

add_library(StbDecoders StbImagePersistance.cpp StbImagePersistance.h StbDecodersModule.cpp StbDecodersModule.h)

target_include_directories(StbDecoders PUBLIC "..")

target_link_libraries(StbDecoders Core Foundation Rendering)

if(BUILD_SHARED_LIBS)
install (TARGETS StbDecoders RUNTIME LIBRARY DESTINATION bin)

    endif()