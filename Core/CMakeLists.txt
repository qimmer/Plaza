cmake_minimum_required(VERSION 3.9)
project(Core)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_dyncall
        GIT_REPOSITORY "https://github.com/Snaipe/dyncall.git"
        UPDATE_COMMAND ""
        INSTALL_COMMAND "")

ExternalProject_Get_Property(ex_dyncall BINARY_DIR)
ExternalProject_Get_Property(ex_dyncall SOURCE_DIR)
SET(ex_dyncall_lib_dir "${BINARY_DIR}")
SET(ex_dyncall_inc_dir "${SOURCE_DIR}")

ADD_LIBRARY(dyncall STATIC IMPORTED)
SET_PROPERTY(TARGET dyncall PROPERTY IMPORTED_LOCATION "${ex_dyncall_lib_dir}/dyncall/libdyncall_s.a")

ExternalProject_Add(ex_eastl
        GIT_REPOSITORY "https://github.com/electronicarts/EASTL.git"
        UPDATE_COMMAND ""
        INSTALL_COMMAND "")

ExternalProject_Get_Property(ex_eastl BINARY_DIR)
ExternalProject_Get_Property(ex_eastl SOURCE_DIR)
SET(ex_eastl_lib_dir "${BINARY_DIR}")
SET(ex_eastl_inc_dir "${SOURCE_DIR}/include")

ADD_LIBRARY(eastl STATIC IMPORTED)
SET_PROPERTY(TARGET eastl PROPERTY IMPORTED_LOCATION "${ex_eastl_lib_dir}/libEASTL.a")

add_library(Core Entity.cpp Entity.h Types.h Type.h Pool.h Handle.h Function.h Delegate.h Debug.cpp Debug.h Handle.cpp Property.h Property.cpp Type.cpp Vector.cpp Vector.h StackContainer.h Dictionary.cpp Dictionary.h String.cpp String.h Variant.h Module.cpp Module.h Service.cpp Service.h CoreModule.cpp CoreModule.h Hierarchy.cpp Hierarchy.h Function.cpp Base64.cpp Base64.h Enum.cpp Enum.h AlignedAllocator.h ShortAlloc.h Hashing.h Hashing.cpp)

add_dependencies(Core ex_dyncall ex_eastl)

target_link_libraries(Core PUBLIC dyncall eastl)
target_include_directories(Core PUBLIC ".." "cglm/include" ${ex_dyncall_inc_dir} ${ex_eastl_inc_dir})
target_compile_options(Core PUBLIC "-fno-rtti" "-fno-exceptions")
