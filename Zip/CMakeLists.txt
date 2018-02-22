cmake_minimum_required(VERSION 3.9)
project(Zip)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_libzip
        GIT_REPOSITORY "https://github.com/nih-at/libzip.git"
        GIT_TAG "master"
        UPDATE_COMMAND ""
        INSTALL_COMMAND "")

ExternalProject_Get_Property(ex_libzip BINARY_DIR)
ExternalProject_Get_Property(ex_libzip SOURCE_DIR)
SET(ex_libzip_lib_dir "${BINARY_DIR}/lib")
SET(ex_libzip_inc_dir "${SOURCE_DIR}/lib")

ADD_LIBRARY(libzip SHARED IMPORTED)
SET_PROPERTY(TARGET libzip PROPERTY IMPORTED_LOCATION "${ex_libzip_lib_dir}/libzip.dylib")

add_library(Zip ZipFileStream.cpp ZipFileStream.h)
target_include_directories(Zip PUBLIC "${SOURCE_DIR}/lib" "${BINARY_DIR}")

add_dependencies(Zip ex_libzip)

target_link_libraries(Zip Core Foundation libzip)
