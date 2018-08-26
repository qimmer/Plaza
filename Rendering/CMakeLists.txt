cmake_minimum_required(VERSION 3.9)
project(Rendering)

set(CMAKE_CXX_STANDARD 14)

add_library(Rendering RenderingModule.cpp RenderingModule.h Program.cpp Program.h Texture.cpp Texture.h Texture2D.cpp Texture2D.h Uniform.cpp Uniform.h CommandList.cpp CommandList.h RenderTarget.cpp RenderTarget.h Batch.cpp Batch.h RenderContext.cpp RenderContext.h Mesh.cpp Mesh.h Material.cpp Material.h SubTexture2D.cpp SubTexture2D.h OffscreenRenderTarget.cpp OffscreenRenderTarget.h ShaderCache.cpp ShaderCache.h Renderer.cpp Renderer.h Renderable.cpp Renderable.h)

target_include_directories(Rendering PUBLIC "..")

target_link_libraries(Rendering Core Foundation Input Json)

if(BUILD_SHARED_LIBS)
    install (TARGETS Rendering RUNTIME LIBRARY DESTINATION bin)

endif()