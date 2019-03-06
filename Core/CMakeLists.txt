cmake_minimum_required(VERSION 3.9)
project(Core)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

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

add_library(Core Entity.cpp Entity.h Types.h Pool.h Function.h Event.h Debug.cpp Debug.h Property.h Property.cpp Vector.cpp Vector.h StackContainer.h Variant.h Module.cpp Module.h CoreModule.cpp CoreModule.h Function.cpp Base64.cpp Base64.h Enum.cpp Enum.h AlignedAllocator.h ShortAlloc.h Hashing.h Hashing.cpp Event.cpp Variant.cpp Component.cpp Component.h NativeUtils.h NativeUtils.cpp Types.cpp Algorithms.h Identification.cpp Identification.h Std.cpp Std.h Date.cpp Date.h Strings.cpp Strings.h Instance.cpp Instance.h Binding.cpp Binding.h NativeFunction.h NativeFunction.cpp FunctionCommonSignatures.h FunctionCommonSignatures.cpp)

add_dependencies(Core ex_eastl)

if(MSVC)
    target_sources(Core PRIVATE Core.def)
endif()

target_link_libraries(Core PUBLIC eastl)

if(UNIX)
    target_link_libraries(Core PUBLIC dl)
    target_sources(Core PRIVATE Unix/ELFModule.cpp Unix/ELFModule.h)
endif()

if(WIN32)
    target_link_libraries(Core PUBLIC ole32 dbghelp)
    target_include_directories(Core PRIVATE libffi/src/x86)
endif()

target_include_directories(Core PUBLIC ".." "cglm/include" "${ex_eastl_inc_dir}" "${ex_eastl_inc_dir}/../test/packages/EABase/include/Common" "${ex_libffi_lib_dir}" "${ex_libffi_inc_dir}" "${ex_libffi_inc_dir}/../src/x86")
target_compile_options(Core PUBLIC "-fno-rtti" "-fno-exceptions" "-std=c++14")
set_property(TARGET Core PROPERTY POSITION_INDEPENDENT_CODE ON)

if(BUILD_SHARED_LIBS)
    install (TARGETS Core RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
endif()