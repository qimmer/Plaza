cmake_minimum_required(VERSION 3.9)
project(Lua)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_luajit
        GIT_REPOSITORY "https://github.com/LuaDist/luajit"
        UPDATE_COMMAND ""
        INSTALL_COMMAND "")

ExternalProject_Get_Property(ex_luajit BINARY_DIR)
ExternalProject_Get_Property(ex_luajit SOURCE_DIR)
SET(ex_luajit_lib_dir "${BINARY_DIR}")
SET(ex_luajit_inc_dir "${SOURCE_DIR}")

ADD_LIBRARY(luajit STATIC IMPORTED)
SET_PROPERTY(TARGET luajit PROPERTY IMPORTED_LOCATION "${ex_luajit_lib_dir}/luajit.a")

add_library(Lua LuaModule.cpp LuaModule.h)

add_dependencies(Lua ex_luajit)

target_link_libraries(Lua PRIVATE luajit)
target_include_directories(Lua PUBLIC ".." "${ex_luajit_inc_dir}/src")
