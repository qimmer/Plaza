//
// Created by Kim Johannsen on 13-03-2018.
//

#include <Rendering/Mesh.h>
#include <Foundation/Stream.h>
#include <Foundation/Task.h>
#include <Rendering/Shader.h>
#include <Rendering/Program.h>
#include <Rendering/Uniform.h>
#include <Rendering/UniformState.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/VertexDeclaration.h>
#include <Rendering/VertexAttribute.h>
#include "VoxelMesh.h"
#include "VoxelPalette.h"
#include "VoxelChunk.h"

#include "../Core/Entity.h"
#include <cglm/cglm.h>

Entity VoxelDataRoot = 0, VoxelVertexShader = 0, VoxelPixelShader = 0, VoxelVertexDeclaration = 0, CubeMesh = 0, VoxelPaletteTextureUniform = 0;
Entity ChunkSizeSizeZUniform = 0, FaceVertexPositionsUsUniform = 0, FaceVertexNormalsVsUniform = 0, PaletteColorUniform = 0, PaletteUvOffsetUniform = 0, PaletteUvSizeUniform = 0;

struct VoxelMesh {
    Entity VoxelMeshChunk, VoxelMeshPalette;
};

struct VoxelVertex {
    rgba8 xyzIndex;
    u16 paletteIndex;
    u8 ao, height;
};

DefineComponent(VoxelMesh)
    Dependency(Mesh)
    DefinePropertyReactive(Entity, VoxelMeshChunk)
    DefinePropertyReactive(Entity, VoxelMeshPalette)
EndComponent()

DefineComponentPropertyReactive(VoxelMesh, Entity, VoxelMeshChunk)
DefineComponentPropertyReactive(VoxelMesh, Entity, VoxelMeshPalette)

Entity GetVoxelDataRoot() {
    return VoxelDataRoot;
}

Entity GetVoxelPaletteTextureUniform() {
    return VoxelPaletteTextureUniform;
}

#define MAX_VERTICES (USHRT_MAX * 8)

static thread_local VoxelVertex verticesBuffer[MAX_VERTICES];

static v3f cubeFaceDirections[] = {
    {1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, -1.0f}
};

static int cubeFaceNeighbourhoodIndices[] = {
    9 * 1 + 3 * 1 + 1 * 2,
    9 * 1 + 3 * 1 + 1 * 0,

    9 * 1 + 3 * 2 + 1 * 1,
    9 * 1 + 3 * 0 + 1 * 1,

    9 * 2 + 3 * 1 + 1 * 1,
    9 * 0 + 3 * 1 + 1 * 1,
};

static v3f cubeFaceUps[] = {
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, -1.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};

static v4f cubeFaceVertexPositionsUs[36];
static v4f cubeFaceVertexNormalsVs[36];
static VoxelVertex cubeVertices[36];

static void CreateCubeVertices() {
    for(auto i = 0; i < 6; ++i) {
        v4f positions[4];
        v4f normals[4];
        v2f uvs[4];

        positions[0] = {0.5f, 0.5f, 0.5f, 1.0f};
        positions[1] = {-0.5f, 0.5f, 0.5f, 1.0f};
        positions[2] = {-0.5f, -0.5f, 0.5f, 1.0f};
        positions[3] = {0.5f, -0.5f, 0.5f, 1.0f};
        normals[0] = normals[1] = normals[2] = normals[3] = {0.0f, 0.0f, 1.0f, 0.0f};

        uvs[0] = {1.0f, 1.0f};
        uvs[1] = {0.0f, 1.0f};
        uvs[2] = {0.0f, 0.0f};
        uvs[3] = {1.0f, 0.0f};

        m4x4f lookMat;
        v3f eye = {0.0f, 0.0f, 0.0f};
        v3f cubeFaceDirNegZ = {
            cubeFaceDirections[i].x,
            cubeFaceDirections[i].y,
            -cubeFaceDirections[i].z
        };
        glm_look(&eye.x, &cubeFaceDirNegZ.x, &cubeFaceUps[i].x, (vec4*)&lookMat.x.x);

        glm_mat4_mulv((vec4*)&lookMat.x.x, &positions[0].x, &positions[0].x);
        glm_mat4_mulv((vec4*)&lookMat.x.x, &positions[1].x, &positions[1].x);
        glm_mat4_mulv((vec4*)&lookMat.x.x, &positions[2].x, &positions[2].x);
        glm_mat4_mulv((vec4*)&lookMat.x.x, &positions[3].x, &positions[3].x);

        glm_mat4_mulv((vec4*)&lookMat.x.x, &normals[0].x, &normals[0].x);
        glm_mat4_mulv((vec4*)&lookMat.x.x, &normals[1].x, &normals[1].x);
        glm_mat4_mulv((vec4*)&lookMat.x.x, &normals[2].x, &normals[2].x);
        glm_mat4_mulv((vec4*)&lookMat.x.x, &normals[3].x, &normals[3].x);

        cubeFaceVertexPositionsUs[i*6 + 0] = {positions[ 0].x + 0.5f, positions[ 0].y + 0.5f, positions[ 0].z + 0.5f, uvs[0].x};
        cubeFaceVertexPositionsUs[i*6 + 1] = {positions[ 1].x + 0.5f, positions[ 1].y + 0.5f, positions[ 1].z + 0.5f, uvs[1].x};
        cubeFaceVertexPositionsUs[i*6 + 2] = {positions[ 2].x + 0.5f, positions[ 2].y + 0.5f, positions[ 2].z + 0.5f, uvs[2].x};
        cubeFaceVertexPositionsUs[i*6 + 3] = {positions[ 2].x + 0.5f, positions[ 2].y + 0.5f, positions[ 2].z + 0.5f, uvs[2].x};
        cubeFaceVertexPositionsUs[i*6 + 4] = {positions[ 3].x + 0.5f, positions[ 3].y + 0.5f, positions[ 3].z + 0.5f, uvs[3].x};
        cubeFaceVertexPositionsUs[i*6 + 5] = {positions[ 0].x + 0.5f, positions[ 0].y + 0.5f, positions[ 0].z + 0.5f, uvs[0].x};

        cubeFaceVertexNormalsVs[i*6 + 0] = {normals[ 0].x, normals[ 0].y, normals[ 0].z, uvs[0].y};
        cubeFaceVertexNormalsVs[i*6 + 1] = {normals[ 1].x, normals[ 1].y, normals[ 1].z, uvs[1].y};
        cubeFaceVertexNormalsVs[i*6 + 2] = {normals[ 2].x, normals[ 2].y, normals[ 2].z, uvs[2].y};
        cubeFaceVertexNormalsVs[i*6 + 3] = {normals[ 2].x, normals[ 2].y, normals[ 2].z, uvs[2].y};
        cubeFaceVertexNormalsVs[i*6 + 4] = {normals[ 3].x, normals[ 3].y, normals[ 3].z, uvs[3].y};
        cubeFaceVertexNormalsVs[i*6 + 5] = {normals[ 0].x, normals[ 0].y, normals[ 0].z, uvs[0].y};

        cubeVertices[i*6 + 0] = {{0, 0, 0, (u8)(i*6 + 0)}, 0, 0, 255};
        cubeVertices[i*6 + 1] = {{0, 0, 0, (u8)(i*6 + 1)}, 0, 0, 255};
        cubeVertices[i*6 + 2] = {{0, 0, 0, (u8)(i*6 + 2)}, 0, 0, 255};
        cubeVertices[i*6 + 3] = {{0, 0, 0, (u8)(i*6 + 3)}, 0, 0, 255};
        cubeVertices[i*6 + 4] = {{0, 0, 0, (u8)(i*6 + 4)}, 0, 0, 255};
        cubeVertices[i*6 + 5] = {{0, 0, 0, (u8)(i*6 + 5)}, 0, 0, 255};
    }

    CubeMesh = CreateMesh(VoxelDataRoot, "CubeMesh");
    auto cubeVertexBuffer = CreateVertexBuffer(CubeMesh, "VertexBuffer");
    SetMeshVertexBuffer(CubeMesh, cubeVertexBuffer);
    SetMeshNumVertices(CubeMesh, 36);
    SetVertexBufferDeclaration(cubeVertexBuffer, VoxelVertexDeclaration);

    Assert(StreamOpen(cubeVertexBuffer, StreamMode_Write));
    StreamWrite(cubeVertexBuffer, sizeof(VoxelVertex) * 36, cubeVertices);
    StreamClose(cubeVertexBuffer);

}

void InitializeVoxelRoot(Entity entity) {
    CreateCubeVertices();

    VoxelVertexDeclaration = CreateVertexDeclaration(entity, "VertexDeclaration");
    auto pos = CreateVertexAttribute(VoxelVertexDeclaration, "Position");
    SetVertexAttributeType(pos, TypeOf_rgba8());
    SetVertexAttributeAsInt(pos, true);
    SetVertexAttributeNormalize(pos, false);
    SetVertexAttributeUsage(pos, VertexAttributeUsage_Position);

    auto texCoord = CreateVertexAttribute(VoxelVertexDeclaration, "TexCoord");
    SetVertexAttributeType(texCoord, TypeOf_rgba8());
    SetVertexAttributeAsInt(texCoord, true);
    SetVertexAttributeNormalize(texCoord, false);
    SetVertexAttributeUsage(texCoord, VertexAttributeUsage_TexCoord0);

    auto shaderDeclaration = CreateStream(entity, "ShaderDeclaration");
    SetStreamPath(shaderDeclaration, "res://voxel/shaders/voxel.var");

    VoxelVertexShader = CreateShader(entity, "VertexShader");
    SetShaderType(VoxelVertexShader, ShaderType_Vertex);
    SetShaderDeclaration(VoxelVertexShader, shaderDeclaration);
    SetStreamPath(VoxelVertexShader, "res://voxel/shaders/voxel.vs");

    VoxelPixelShader = CreateShader(entity, "PixelShader");
    SetShaderType(VoxelPixelShader, ShaderType_Pixel);
    SetShaderDeclaration(VoxelPixelShader, shaderDeclaration);
    SetStreamPath(VoxelPixelShader, "res://voxel/shaders/voxel.ps");

    FaceVertexPositionsUsUniform = CreateUniform(entity, "VertexPositionsUsUniform");
    SetUniformName(FaceVertexPositionsUsUniform, "voxel_face_vertex_positions_us");
    SetUniformType(FaceVertexPositionsUsUniform, TypeOf_v4f());
    SetUniformArrayCount(FaceVertexPositionsUsUniform, 36);
    SetUniformStateUniform(FaceVertexPositionsUsUniform, FaceVertexPositionsUsUniform);
    SetUniformStateState(FaceVertexPositionsUsUniform, sizeof(v4f) * 36, cubeFaceVertexPositionsUs);

    FaceVertexNormalsVsUniform = CreateUniform(entity, "VertexNormalsVsUniform");
    SetUniformName(FaceVertexNormalsVsUniform, "voxel_face_vertex_normals_vs");
    SetUniformType(FaceVertexNormalsVsUniform, TypeOf_v4f());
    SetUniformArrayCount(FaceVertexNormalsVsUniform, 36);
    SetUniformStateUniform(FaceVertexNormalsVsUniform, FaceVertexNormalsVsUniform);
    SetUniformStateState(FaceVertexNormalsVsUniform, sizeof(v4f) * 36, cubeFaceVertexNormalsVs);

    VoxelPaletteTextureUniform = CreateUniform(entity, "VoxelPaletteTextureUniform");
    SetUniformType(VoxelPaletteTextureUniform, TypeOf_Entity());
    SetUniformName(VoxelPaletteTextureUniform, "s_palette");
}

static void GenerateVertices(const Voxel *voxels, v3i bufferSize, v3i start, v3i end, u32 *vertexCountOut, Entity stream) {
    auto vertexCount = 0;

    auto strideZ = bufferSize.x * bufferSize.y;
    auto strideY = bufferSize.x;

    for(auto vz = start.z; vz < end.z; ++vz) {
        int offsetZ[] = {
            strideZ * (vz - 1),
            strideZ * (vz),
            strideZ * (vz + 1),
        };

        for(auto vy = start.y; vy < end.y; ++vy) {
            int offsetY[] = {
                strideY * (vy - 1),
                strideY * (vy),
                strideY * (vy + 1),
            };

            for(auto vx = start.x; vx < end.x; ++vx) {
                int offsetX[] = {
                    (vx - 1),
                    (vx),
                    (vx + 1),
                };

                auto centerBufferIndex = offsetZ[1] + offsetY[1] + offsetX[1];
                auto centerVoxel = voxels[centerBufferIndex];

                // Only handle this voxel if it is solid
                if(!centerVoxel) continue;

                if(vertexCount >= (MAX_VERTICES - 36)){
                    *vertexCountOut = 0;
                    Log(LogChannel_Core, LogSeverity_Error, "Number of vertices exceeds MAX_VERTICES for voxel chunks. Voxel mesh not generated.");
                    return;
                }

                const Voxel neighbourHood[27] = {
                    voxels[offsetZ[0] + offsetY[0] + offsetX[0]],
                    voxels[offsetZ[0] + offsetY[0] + offsetX[1]],
                    voxels[offsetZ[0] + offsetY[0] + offsetX[2]],

                    voxels[offsetZ[0] + offsetY[1] + offsetX[0]],
                    voxels[offsetZ[0] + offsetY[1] + offsetX[1]],
                    voxels[offsetZ[0] + offsetY[1] + offsetX[2]],

                    voxels[offsetZ[0] + offsetY[2] + offsetX[0]],
                    voxels[offsetZ[0] + offsetY[2] + offsetX[1]],
                    voxels[offsetZ[0] + offsetY[2] + offsetX[2]],

                    voxels[offsetZ[1] + offsetY[0] + offsetX[0]],
                    voxels[offsetZ[1] + offsetY[0] + offsetX[1]],
                    voxels[offsetZ[1] + offsetY[0] + offsetX[2]],

                    voxels[offsetZ[1] + offsetY[1] + offsetX[0]],
                    voxels[offsetZ[1] + offsetY[1] + offsetX[1]],
                    voxels[offsetZ[1] + offsetY[1] + offsetX[2]],

                    voxels[offsetZ[1] + offsetY[2] + offsetX[0]],
                    voxels[offsetZ[1] + offsetY[2] + offsetX[1]],
                    voxels[offsetZ[1] + offsetY[2] + offsetX[2]],

                    voxels[offsetZ[2] + offsetY[0] + offsetX[0]],
                    voxels[offsetZ[2] + offsetY[0] + offsetX[1]],
                    voxels[offsetZ[2] + offsetY[0] + offsetX[2]],

                    voxels[offsetZ[2] + offsetY[1] + offsetX[0]],
                    voxels[offsetZ[2] + offsetY[1] + offsetX[1]],
                    voxels[offsetZ[2] + offsetY[1] + offsetX[2]],

                    voxels[offsetZ[2] + offsetY[2] + offsetX[0]],
                    voxels[offsetZ[2] + offsetY[2] + offsetX[1]],
                    voxels[offsetZ[2] + offsetY[2] + offsetX[2]]
                };

                v3i voxelPosition = {
                    vx - start.x,
                    vy - start.y,
                    vz - start.z
                };

                for(auto face = 0; face < 6; ++face) {
                    // If neighbour is air, add face
                    auto neighbourHoodIndex = cubeFaceNeighbourhoodIndices[face];
                    if(!neighbourHood[neighbourHoodIndex]) {
                        for(auto faceVertexIndex = 0; faceVertexIndex < 6; ++faceVertexIndex) {
                            verticesBuffer[vertexCount++] = {
                                {
                                        (u8)voxelPosition.x,
                                        (u8)voxelPosition.y,
                                        (u8)voxelPosition.z,
                                        (u8)(face * 6 + faceVertexIndex),
                                },
                                centerVoxel,
                                0,
                                255
                            };
                        }
                    }
                }
            }
        }
    }

    *vertexCountOut = vertexCount;

    StreamWrite(stream, vertexCount * sizeof(VoxelVertex), verticesBuffer);
}

static void RegenerateVoxelMesh(Entity voxelMesh) {
    auto vertexBuffer = GetMeshVertexBuffer(voxelMesh);
    auto voxelChunk = GetVoxelMeshChunk(voxelMesh);

    if(!IsEntityValid(vertexBuffer) || !IsEntityValid(voxelChunk)) return;

    auto chunkSize = GetVoxelChunkSize(voxelChunk);
    v3i bufferSize = {chunkSize.x + 2, chunkSize.y + 2, chunkSize.z + 2};
    auto bufferBytes = bufferSize.x * bufferSize.y * bufferSize.z * sizeof(Voxel);
    auto buffer = (Voxel*)malloc(bufferBytes);
    memset(buffer, 0, bufferBytes);

    GetVoxelChunkRegion(voxelChunk, {0, 0, 0}, chunkSize, {1, 1, 1}, bufferSize, buffer);

    auto threadIndex = GetCurrentThreadIndex();

    Assert(StreamOpen(vertexBuffer, StreamMode_Write));
    u32 vertexCount = 0;
    GenerateVertices(buffer, bufferSize, {1, 1, 1}, {bufferSize.x - 1, bufferSize.y - 1, bufferSize.z - 1}, &vertexCount, vertexBuffer);
    StreamClose(vertexBuffer);

    SetMeshNumIndices(voxelMesh, 0);
    SetMeshNumVertices(voxelMesh, vertexCount);
    SetMeshIndexBuffer(voxelMesh, 0);
    SetMeshStartIndex(voxelMesh, 0);
    SetMeshStartVertex(voxelMesh, 0);

    free(buffer);
}

static void OnVoxelChunkChanged(Entity voxelChunk) {
    if(HasVoxelChunk(voxelChunk)) {
        for_entity(voxelMesh, VoxelMesh) {
            if(GetVoxelMeshChunk(voxelMesh) == voxelChunk) RegenerateVoxelMesh(voxelMesh);
        }
    }
}

static void OnVoxelPaletteChanged(Entity voxelPalette) {
    for_entity(voxelMesh, VoxelMesh) {
        if(GetVoxelMeshPalette(voxelMesh) == voxelPalette) RegenerateVoxelMesh(voxelMesh);
    }
}

static void OnVoxelMeshAdded(Entity voxelMesh) {
    auto vb = CreateVertexBuffer(voxelMesh, "VertexBuffer");
    SetMeshVertexBuffer(voxelMesh, vb);
    SetVertexBufferDeclaration(vb, VoxelVertexDeclaration);
}

DefineService(VoxelMesh)
    Subscribe(VoxelMeshAdded, OnVoxelMeshAdded)
    Subscribe(VoxelPaletteChanged, OnVoxelPaletteChanged)
    Subscribe(VoxelChunkChanged, OnVoxelChunkChanged)
    Subscribe(VoxelMeshChanged, RegenerateVoxelMesh)
    Subscribe(StreamContentChanged, OnVoxelChunkChanged)
    Subscribe(StreamChanged, OnVoxelChunkChanged)
    ServiceEntity(VoxelDataRoot, InitializeVoxelRoot)
EndService()

Entity GetVoxelVertexShader() {
    return VoxelVertexShader;
}

Entity GetVoxelPixelShader() {
    return VoxelPixelShader;
}
