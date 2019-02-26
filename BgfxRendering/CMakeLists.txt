cmake_minimum_required(VERSION 3.9)
project(BgfxRendering)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(ex_bgfx
        GIT_REPOSITORY "https://github.com/JoshuaBrookover/bgfx.cmake.git"
        GIT_TAG "master"
        UPDATE_COMMAND ""
        CMAKE_ARGS "${CMAKE_ARGS};-DBGFX_INSTALL=ON;-DBGFX_USE_DEBUG_SUFFIX=OFF;-DBGFX_BUILD_EXAMPLES=OFF;-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=Release;-DBGFX_CONFIG_DEBUG=OFF;-DBGFX_CONFIG_MULTITHREADED=On;-DBUILD_SHARED_LIBS=OFF;-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true")

ExternalProject_Get_Property(ex_bgfx BINARY_DIR)
ExternalProject_Get_Property(ex_bgfx SOURCE_DIR)
SET(ex_bgfx_lib_dir "${BINARY_DIR}")
SET(ex_bgfx_inc_dir "${SOURCE_DIR}/bgfx/include;${SOURCE_DIR}/bx/include;${SOURCE_DIR}/bimg/include")
SET(ex_bgfx_shader_dir "${SOURCE_DIR}/bgfx/examples/common")
SET(ex_bgfx_src_dir "${SOURCE_DIR}/bgfx/src")

ADD_LIBRARY(bx STATIC IMPORTED)
SET_PROPERTY(TARGET bx PROPERTY IMPORTED_LOCATION "${ex_bgfx_lib_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}bx${CMAKE_STATIC_LIBRARY_SUFFIX}")
ADD_LIBRARY(bimg STATIC IMPORTED)
SET_PROPERTY(TARGET bimg PROPERTY IMPORTED_LOCATION "${ex_bgfx_lib_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}bimg${CMAKE_STATIC_LIBRARY_SUFFIX}")
ADD_LIBRARY(bgfx STATIC IMPORTED)
SET_PROPERTY(TARGET bgfx PROPERTY IMPORTED_LOCATION "${ex_bgfx_lib_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}bgfx${CMAKE_STATIC_LIBRARY_SUFFIX}")

ExternalProject_Add(ex_glfw
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "master"
        UPDATE_COMMAND ""
        CMAKE_ARGS "${CMAKE_ARGS};-DGLFW_BUILD_EXAMPLES=OFF;-DGLFW_BUILD_DOCS=OFF;-DGLFW_BUILD_TESTS=OFF;-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=Release;-DBUILD_SHARED_LIBS=OFF")


ExternalProject_Get_Property(ex_glfw BINARY_DIR)
ExternalProject_Get_Property(ex_glfw SOURCE_DIR)
SET(ex_glfw_lib_dir "${BINARY_DIR}/src")
SET(ex_glfw_inc_dir "${SOURCE_DIR}/include")

ADD_LIBRARY(glfw STATIC IMPORTED)
SET_PROPERTY(TARGET glfw PROPERTY IMPORTED_LOCATION "${ex_glfw_lib_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}")

add_library(BgfxRendering BgfxModule.cpp BgfxModule.h BgfxModule.cpp BgfxModule.h BgfxRenderContext.cpp BgfxRenderContext.h BgfxCommandList.cpp BgfxCommandList.h BgfxShaderCache.cpp BgfxShaderCache.h BgfxUniform.cpp BgfxUniform.h BgfxTexture2D.cpp BgfxTexture2D.h BgfxMesh.cpp BgfxMesh.h BgfxOffscreenRenderTarget.cpp BgfxOffscreenRenderTarget.h BgfxMesh.cpp BgfxMesh.h BgfxResource.cpp BgfxResource.h BgfxTextureReadBack.cpp BgfxTextureReadBack.h)
add_dependencies(BgfxRendering ex_bgfx ex_glfw)

target_include_directories(BgfxRendering PUBLIC ".." ${ex_bgfx_inc_dir} ${ex_glfw_inc_dir})

SET(libs Core Foundation Rendering glfw)

SET(libs ${libs} bgfx bimg bx)

IF(WIN32)
    SET(libs ${libs} psapi)
ENDIF()

IF(APPLE)
    find_library( COCOA_LIBRARY Cocoa )
    find_library( METAL_LIBRARY Metal )
    find_library( QUARTZCORE_LIBRARY QuartzCore )
    mark_as_advanced( COCOA_LIBRARY )
    mark_as_advanced( METAL_LIBRARY )
    mark_as_advanced( QUARTZCORE_LIBRARY )
    SET(libs ${libs} "-framework IOKit -framework CoreFoundation -framework CoreVideo" ${COCOA_LIBRARY} ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
ENDIF()

IF(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    SET(libs ${libs} ${X11_X11_LIB} ${X11_Xrandr_LIB} ${X11_Xinerama_LIB} ${X11_Xkb_LIB} ${X11_Xcursor_LIB} ${X11_Xinput_LIB})
ENDIF()

target_link_libraries(BgfxRendering ${libs})

IF(WIN32 OR APPLE OR UNIX)
    SET(WITH_SHADER_COMPILER ON)
ENDIF()

IF(WITH_SHADER_COMPILER)
    target_sources(BgfxRendering PRIVATE BgfxShaderCompiler.cpp BgfxShaderCompiler.h)
    target_compile_definitions(BgfxRendering PUBLIC "BGFXRENDERING_WITH_SHADER_COMPILER")
    target_link_libraries(BgfxRendering File)
ENDIF(WITH_SHADER_COMPILER)

INSTALL(FILES ${ex_bgfx_shader_dir}/shaderlib.sh ${ex_bgfx_shader_dir}/common.sh ${ex_bgfx_src_dir}/bgfx_shader.sh ${ex_bgfx_src_dir}/bgfx_compute.sh DESTINATION bin/Assets/Shaders)

if(BUILD_SHARED_LIBS)
install (TARGETS BgfxRendering RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

    endif()