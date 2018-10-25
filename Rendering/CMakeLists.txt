cmake_minimum_required(VERSION 3.9)
project(Rendering)

set(CMAKE_CXX_STANDARD 14)

add_library(Rendering RenderingModule.cpp RenderingModule.h Program.cpp Program.h Texture.cpp Texture.h Texture2D.cpp Texture2D.h Uniform.cpp Uniform.h RenderTarget.cpp RenderTarget.h RenderContext.cpp RenderContext.h Mesh.cpp Mesh.h Material.cpp Material.h OffscreenRenderTarget.cpp OffscreenRenderTarget.h ShaderCache.cpp ShaderCache.h RenderState.cpp RenderState.h Renderable.cpp Renderable.h SceneRenderer.h SceneRenderer.cpp MeshBuilder.cpp MeshBuilder.h)

target_include_directories(Rendering PUBLIC "..")

target_link_libraries(Rendering Core Foundation Input Json Scene)

if(BUILD_SHARED_LIBS)
    install (TARGETS Rendering RUNTIME DESTINATION bin LIBRARY DESTINATION bin)

endif()