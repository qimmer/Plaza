cmake_minimum_required(VERSION 3.9)
project(Rendering)

set(CMAKE_CXX_STANDARD 14)

add_library(Rendering RenderingModule.cpp RenderingModule.h Shader.cpp Shader.h Program.cpp Program.h Texture.cpp Texture.h Texture2D.cpp Texture2D.h Uniform.cpp Uniform.h CommandList.cpp CommandList.h RenderTarget.cpp RenderTarget.h Batch.cpp Batch.h VertexBuffer.cpp VertexBuffer.h IndexBuffer.cpp IndexBuffer.h VertexAttribute.cpp VertexAttribute.h Context.cpp Context.h BinaryShader.cpp BinaryShader.h ShaderCompiler.cpp ShaderCompiler.h Mesh.cpp Mesh.h Material.cpp Material.h UniformState.cpp UniformState.h SubTexture2D.cpp SubTexture2D.h VertexDeclaration.cpp VertexDeclaration.h OffscreenRenderTarget.cpp OffscreenRenderTarget.h ShaderVariation.cpp ShaderVariation.h)

target_include_directories(Rendering PUBLIC "..")

target_link_libraries(Rendering Core Foundation Input)