cmake_minimum_required(VERSION 3.9)
project(Json)

set(CMAKE_CXX_STANDARD 14)

add_library(Json JsonPersistance.h JsonPersistance.cpp JsonModule.cpp JsonModule.h NativeUtils.h)

target_include_directories(Json PUBLIC ".." ".")

target_link_libraries(Json Foundation Core)

if(MSVC)
    target_sources(Json PRIVATE Json.def)
endif()

if(BUILD_SHARED_LIBS)
install (TARGETS Json RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

    endif()