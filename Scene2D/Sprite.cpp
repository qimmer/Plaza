//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include <Rendering/Mesh.h>
#include <Foundation/AppLoop.h>
#include <Rendering/VertexBuffer.h>
#include <Rendering/VertexAttribute.h>
#include <Core/Hierarchy.h>
#include <Rendering/Program.h>
#include <Rendering/Shader.h>
#include <File/FileStream.h>
#include <Rendering/Uniform.h>
#include <Core/String.h>
#include <Foundation/Stream.h>
#include <Rendering/IndexBuffer.h>
#include <Rendering/VertexDeclaration.h>
#include "Sprite.h"

struct SpriteVertex {
    v2f Position, Uv;
};

static Entity SpriteVertexDeclaration,
        SpriteProgram,
        SpriteTextureUniform,
        SpriteVertexBuffer,
        SpriteIndexBuffer,
        SpriteMesh,
        SpriteRoot;

DefineService(Sprite)
EndService()

Entity GetSpriteTextureUniform() {
    return SpriteTextureUniform;
}

Entity GetSpriteMesh() {
    return SpriteMesh;
}

Entity GetSpriteProgram() {
    return SpriteProgram;
}

static void InitializeSpriteMesh() {
    v2f uvTopLeft = {0.0f, 1.0f};
    v2f uvBottomRight = {1.0f, 0.0f};
    v2f posTopLeft = {-0.5f, -0.5f};
    v2f posBottomRight = {0.5f, 0.5f};

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/Mesh", GetEntityPath(SpriteRoot));
    SpriteMesh = CreateMesh(path);

    char vbpath[PATH_MAX];
    snprintf(vbpath, PATH_MAX, "%s/VertexBuffer", GetEntityPath(SpriteRoot));
    SpriteVertexBuffer = CreateVertexBuffer(vbpath);
    SetVertexBufferDeclaration(SpriteVertexBuffer, SpriteVertexDeclaration);
    SetMeshVertexBuffer(SpriteMesh, SpriteVertexBuffer);

    char ibpath[PATH_MAX];
    snprintf(ibpath, PATH_MAX, "%s/IndexBuffer", GetEntityPath(SpriteRoot));
    SpriteIndexBuffer = CreateIndexBuffer(ibpath);
    SetMeshIndexBuffer(SpriteMesh, SpriteIndexBuffer);

    SpriteVertex vts[] = {
        {{posTopLeft.x, posTopLeft.y}, {uvTopLeft.x, uvTopLeft.y}},
        {{posBottomRight.x, posTopLeft.y}, {uvBottomRight.x, uvTopLeft.y}},
        {{posBottomRight.x, posBottomRight.y}, {uvBottomRight.x, uvBottomRight.y}},
        {{posTopLeft.x, posBottomRight.y}, {uvTopLeft.x, uvBottomRight.y}},
    };

    u16 idx[] = {0, 1, 2, 2, 3, 0};

    SetMeshNumVertices(SpriteMesh, 4);
    SetMeshNumIndices(SpriteMesh, 6);

    Assert(StreamOpen(SpriteVertexBuffer, StreamMode_Write));
    StreamWrite(SpriteVertexBuffer, sizeof(SpriteVertex) * 4, vts);
    StreamClose(SpriteVertexBuffer);

    Assert(StreamOpen(SpriteIndexBuffer, StreamMode_Write));
    StreamWrite(SpriteIndexBuffer, sizeof(u16) * 6, idx);
    StreamClose(SpriteIndexBuffer);
}

static void InitializeSpriteVertexDeclaration() {
    char declPath[PATH_MAX];
    char posPath[PATH_MAX];
    char texPath[PATH_MAX];

    snprintf(declPath, PATH_MAX, "%s/VertexDeclaration", GetEntityPath(SpriteRoot));
    snprintf(posPath, PATH_MAX, "%s/Position", declPath);
    snprintf(texPath, PATH_MAX, "%s/TexCoord", declPath);

    SpriteVertexDeclaration = CreateVertexDeclaration(declPath);

    auto positionAttribute = CreateVertexAttribute(posPath);
    SetVertexAttributeUsage(positionAttribute, VertexAttributeUsage_Position);
    SetVertexAttributeType(positionAttribute, TypeOf_v2f());

    auto uvAttribute = CreateVertexAttribute(texPath);
    SetVertexAttributeUsage(uvAttribute, VertexAttributeUsage_TexCoord0);
    SetVertexAttributeType(uvAttribute, TypeOf_v2f());
}

static void InitializeSpriteProgram() {
    char programPath[PATH_MAX];
    char vsPath[PATH_MAX];
    char psPath[PATH_MAX];
    char ufPath[PATH_MAX];

    snprintf(programPath, PATH_MAX, "%s/Program", GetEntityPath(SpriteRoot));
    snprintf(vsPath, PATH_MAX, "%s/VertexShader", programPath);
    snprintf(psPath, PATH_MAX, "%s/PixelShader", programPath);
    snprintf(ufPath, PATH_MAX, "%s/TextureUniform", GetEntityPath(SpriteRoot));

    SpriteProgram = CreateProgram(programPath);
    auto VertexShader = CreateShader(vsPath);
    auto PixelShader = CreateShader(psPath);
    SpriteTextureUniform = CreateUniform(ufPath);

    SetStreamPath(VertexShader, "res://scene2d/shaders/sprite.vs");
    SetStreamPath(PixelShader, "res://scene2d/shaders/sprite.ps");
    SetStreamPath(SpriteProgram, "res://scene2d/shaders/sprite.var");

    SetShaderType(VertexShader, ShaderType_Vertex);
    SetShaderType(PixelShader, ShaderType_Pixel);

    SetUniformName(SpriteTextureUniform, "s_tex");
    SetUniformType(SpriteTextureUniform, TypeOf_Entity());
    SetUniformArrayCount(SpriteTextureUniform, 1);
}

static bool ServiceStart() {
    SpriteRoot = CreateHierarchy("/.Sprite");

    InitializeSpriteProgram();
    InitializeSpriteVertexDeclaration();
    InitializeSpriteMesh();

    return true;
}

static bool ServiceStop() {
    DestroyEntity(SpriteRoot);

    return true;
}
