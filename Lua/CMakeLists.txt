cmake_minimum_required(VERSION 3.9)
project(LuaScripting)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_luajit
        GIT_REPOSITORY "https://github.com/LuaDist/luajit"
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=Release;-DBUILD_SHARED_LIBS=OFF")

ExternalProject_Get_Property(ex_luajit BINARY_DIR)
ExternalProject_Get_Property(ex_luajit SOURCE_DIR)
SET(ex_luajit_lib_dir "${BINARY_DIR}")
SET(ex_luajit_inc_dir "${SOURCE_DIR}")

ADD_LIBRARY(luajit STATIC IMPORTED)
SET_PROPERTY(TARGET luajit PROPERTY IMPORTED_LOCATION "${ex_luajit_lib_dir}/liblua.dll.a")

add_library(LuaScripting LuaModule.cpp LuaModule.h)

add_dependencies(LuaScripting ex_luajit)

target_link_libraries(LuaScripting PRIVATE Core Foundation luajit)
target_include_directories(LuaScripting PUBLIC ".." "${ex_luajit_inc_dir}/src")
