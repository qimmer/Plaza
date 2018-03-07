//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include <Rendering/Mesh.h>
#include <Foundation/Invalidation.h>
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
    v2f uvTopLeft = {0.0f, 0.0f};
    v2f uvBottomRight = {1.0f, 1.0f};
    v2f posTopLeft = {-0.5f, -0.5f};
    v2f posBottomRight = {0.5f, 0.5f};

    SpriteMesh = CreateMesh(FormatString("%s/Mesh", GetEntityPath(SpriteRoot)));

    SpriteVertexBuffer = CreateVertexBuffer(FormatString("%s/VertexBuffer", GetEntityPath(SpriteRoot)));
    SetVertexBufferDeclaration(SpriteVertexBuffer, SpriteVertexDeclaration);
    SetMeshVertexBuffer(SpriteMesh, SpriteVertexBuffer);

    SpriteIndexBuffer = CreateIndexBuffer(FormatString("%s/IndexBuffer", GetEntityPath(SpriteRoot)));
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
    SpriteVertexDeclaration = CreateVertexDeclaration(FormatString("%s/VertexDeclaration", GetEntityPath(SpriteRoot)));

    auto positionAttribute = CreateVertexAttribute(FormatString("%s/Position", GetEntityPath(SpriteVertexDeclaration)));
    SetVertexAttributeUsage(positionAttribute, VertexAttributeUsage_Position);
    SetVertexAttributeType(positionAttribute, TypeOf_v2f());

    auto uvAttribute = CreateVertexAttribute(FormatString("%s/TexCoord", GetEntityPath(SpriteVertexDeclaration)));
    SetVertexAttributeUsage(uvAttribute, VertexAttributeUsage_TexCoord0);
    SetVertexAttributeType(uvAttribute, TypeOf_v2f());
}

static void InitializeSpriteProgram() {
    SpriteProgram = CreateProgram(FormatString("%s/Program", GetEntityPath(SpriteRoot)));
    auto VertexShader = CreateShader(FormatString("%s/VertexShader", GetEntityPath(SpriteProgram)));
    auto PixelShader = CreateShader(FormatString("%s/PixelShader", GetEntityPath(SpriteProgram)));
    SpriteTextureUniform = CreateUniform(FormatString("%s/TextureUniform", GetEntityPath(SpriteRoot)));

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
