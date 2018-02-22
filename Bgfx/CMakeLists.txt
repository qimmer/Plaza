cmake_minimum_required(VERSION 3.9)
project(Bgfx)

add_subdirectory(glfw)

OPTION(WITH_SHADER_COMPILER "Build with BGFX shader compiler intergration" ON)

if( APPLE )
    set( CMAKE_CXX_FLAGS "-ObjC++ --std=c++11" )
elseif(UNIX)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set( BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bx")
set( BIMG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bimg")
set( BGFX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bgfx")

include( cmake/shared.cmake )
include( cmake/bx.cmake )
include( cmake/bimg.cmake )
include( cmake/bgfx.cmake )
include( cmake/tools/shaderc.cmake )

add_definitions("-DBGFX_CONFIG_RENDERER_OPENGL=41")

add_library(Bgfx shaderc.cpp BgfxModule.cpp BgfxModule.h BgfxModule.cpp BgfxModule.h BgfxContext.cpp BgfxContext.h BgfxCommandList.cpp BgfxCommandList.h BgfxVertexBuffer.cpp BgfxVertexBuffer.h BgfxIndexBuffer.cpp BgfxIndexBuffer.h BgfxProgram.cpp BgfxProgram.h BgfxBinaryShader.cpp BgfxBinaryShader.h BgfxUniform.cpp BgfxUniform.h BgfxTexture2D.cpp BgfxTexture2D.h BgfxShaderCompiler.cpp BgfxShaderCompiler.h)

target_include_directories(Bgfx PUBLIC ".." "bx/include" "bimg/include" "bgfx/include" "glfw/include")
target_link_libraries(Bgfx Core Foundation Rendering bx bgfxlib bimg glfw)

IF(WITH_SHADER_COMPILER)
    target_sources(Bgfx PRIVATE "bgfx/tools/shaderc/shaderc_glsl.cpp" "bgfx/tools/shaderc/shaderc_hlsl.cpp" "bgfx/tools/shaderc/shaderc_pssl.cpp")
    target_compile_definitions(Bgfx PUBLIC "WITH_SHADER_COMPILER")
    target_link_libraries(Bgfx bgfx-vertexdecl bgfx-shader-spirv fcpp glsl-optimizer glslang File)
ENDIF(WITH_SHADER_COMPILER)