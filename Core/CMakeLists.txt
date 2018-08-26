cmake_minimum_required(VERSION 3.9)
project(Core)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_dyncall
        GIT_REPOSITORY "https://github.com/Snaipe/dyncall.git"
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE};-DBUILD_SHARED_LIBS=OFF")

ExternalProject_Get_Property(ex_dyncall BINARY_DIR)
ExternalProject_Get_Property(ex_dyncall SOURCE_DIR)
SET(ex_dyncall_lib_dir "${BINARY_DIR}")
SET(ex_dyncall_inc_dir "${SOURCE_DIR}")

ADD_LIBRARY(dyncall STATIC IMPORTED)

IF(MSVC)
SET_PROPERTY(TARGET dyncall PROPERTY IMPORTED_LOCATION "${ex_dyncall_lib_dir}/dyncall/${CMAKE_BUILD_TYPE}/dyncall_s.lib")
ELSE()
SET_PROPERTY(TARGET dyncall PROPERTY IMPORTED_LOCATION "${ex_dyncall_lib_dir}/dyncall/libdyncall_s.a")
ENDIF()
ExternalProject_Add(ex_eastl
        GIT_REPOSITORY "https://github.com/electronicarts/EASTL.git"
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_ARGS "${CMAKE_ARGS};-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE};-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true")

ExternalProject_Get_Property(ex_eastl BINARY_DIR)
ExternalProject_Get_Property(ex_eastl SOURCE_DIR)
SET(ex_eastl_lib_dir "${BINARY_DIR}")
SET(ex_eastl_inc_dir "${SOURCE_DIR}/include")

ADD_LIBRARY(eastl STATIC IMPORTED)

IF(MSVC)
SET_PROPERTY(TARGET eastl PROPERTY IMPORTED_LOCATION "${ex_eastl_lib_dir}/${CMAKE_BUILD_TYPE}/EASTL.lib")
ELSE()
SET_PROPERTY(TARGET eastl PROPERTY IMPORTED_LOCATION "${ex_eastl_lib_dir}/libEASTL.a")
ENDIF()

add_library(Core Entity.cpp Entity.h Types.h Pool.h Function.h Event.h Debug.cpp Debug.h Property.h Property.cpp Vector.cpp Vector.h StackContainer.h Variant.h Module.cpp Module.h CoreModule.cpp CoreModule.h Function.cpp Base64.cpp Base64.h Enum.cpp Enum.h AlignedAllocator.h ShortAlloc.h Hashing.h Hashing.cpp Event.cpp Variant.cpp Component.cpp Component.h NativeUtils.h NativeUtils.cpp Types.cpp Math.h Identification.cpp Identification.h Std.cpp Std.h)

add_dependencies(Core ex_dyncall ex_eastl)

if(MSVC)
    target_sources(Core PRIVATE Core.def)
endif()

target_link_libraries(Core PUBLIC dyncall eastl)

if(UNIX)
    target_link_libraries(Core PUBLIC dl)
    target_sources(Core PRIVATE Unix/ELFModule.cpp Unix/ELFModule.h)
endif()

target_include_directories(Core PUBLIC ".." "cglm/include" "${ex_dyncall_inc_dir}" "${ex_eastl_inc_dir}" "${ex_eastl_inc_dir}/../test/packages/EABase/include/Common")
target_compile_options(Core PUBLIC "-fno-rtti" "-fno-exceptions" "-std=c++14")
set_property(TARGET Core PROPERTY POSITION_INDEPENDENT_CODE ON)

if(BUILD_SHARED_LIBS)
    install (TARGETS Core RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()