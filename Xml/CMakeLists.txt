cmake_minimum_required(VERSION 3.9)
project(Xml)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_expat
        GIT_REPOSITORY "https://github.com/libexpat/libexpat.git"
        GIT_TAG "master"
        SOURCE_SUBDIR "expat"
        UPDATE_COMMAND ""
        INSTALL_COMMAND "")

ExternalProject_Get_Property(ex_expat BINARY_DIR)
ExternalProject_Get_Property(ex_expat SOURCE_DIR)
SET(ex_expat_lib_dir "${BINARY_DIR}")
SET(ex_expat_inc_dir "${SOURCE_DIR}/expat/lib")

ADD_LIBRARY(expat SHARED IMPORTED XmlLoader.cpp XmlLoader.h)
SET_PROPERTY(TARGET expat PROPERTY IMPORTED_LOCATION "${ex_expat_lib_dir}/libexpat.dylib")

add_library(Xml XmlLoader.cpp XmlLoader.h)
target_include_directories(Xml PUBLIC ${ex_expat_inc_dir})

add_dependencies(Xml ex_expat)

target_link_libraries(Xml Core Foundation expat)
