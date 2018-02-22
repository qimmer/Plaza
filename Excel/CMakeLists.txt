cmake_minimum_required(VERSION 3.9)
project(Excel)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_xlsxio
        GIT_REPOSITORY "https://github.com/brechtsanders/xlsxio.git"
        GIT_TAG "master"
        CMAKE_ARGS "-DBUILD_EXAMPLES=FALSE"
        UPDATE_COMMAND ""
        INSTALL_COMMAND "")

ExternalProject_Get_Property(ex_xlsxio BINARY_DIR)
ExternalProject_Get_Property(ex_xlsxio SOURCE_DIR)
SET(ex_xlsxio_lib_dir "${BINARY_DIR}")
SET(ex_xlsxio_inc_dir "${SOURCE_DIR}/include")

ADD_LIBRARY(xlsxio SHARED IMPORTED)
SET_PROPERTY(TARGET xlsxio PROPERTY IMPORTED_LOCATION "${ex_xlsxio_lib_dir}/libxlsxio_read.dylib")

add_library(Excel ExcelLoader.cpp ExcelLoader.h)
target_include_directories(Excel PUBLIC ${ex_xlsxio_inc_dir} ${ex_xlsxio_lib_dir})

add_dependencies(Excel ex_xlsxio)

target_link_libraries(Excel Core Foundation Xml Zip xlsxio)
