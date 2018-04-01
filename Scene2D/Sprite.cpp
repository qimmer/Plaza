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
        SpriteVertexShader,
        SpritePixelShader,
        SpriteTextureUniform,
        SpriteVertexBuffer,
        SpriteIndexBuffer,
        SpriteMesh,
        SpriteRoot;

Entity GetSpriteTextureUniform() {
    return SpriteTextureUniform;
}

Entity GetSpriteMesh() {
    return SpriteMesh;
}

static void CreateSpriteMesh(Entity entity) {
    v2f uvTopLeft = {0.0f, 1.0f};
    v2f uvBottomRight = {1.0f, 0.0f};
    v2f posTopLeft = {-0.5f, -0.5f};
    v2f posBottomRight = {0.5f, 0.5f};

    SpriteVertexBuffer = CreateVertexBuffer(entity, "VertexBuffer");
    SetVertexBufferDeclaration(SpriteVertexBuffer, SpriteVertexDeclaration);
    SetMeshVertexBuffer(entity, SpriteVertexBuffer);

    SpriteIndexBuffer = CreateIndexBuffer(entity, "IndexBuffer");
    SetMeshIndexBuffer(entity, SpriteIndexBuffer);

    SpriteVertex vts[] = {
        {{posTopLeft.x, posTopLeft.y}, {uvTopLeft.x, uvTopLeft.y}},
        {{posBottomRight.x, posTopLeft.y}, {uvBottomRight.x, uvTopLeft.y}},
        {{posBottomRight.x, posBottomRight.y}, {uvBottomRight.x, uvBottomRight.y}},
        {{posTopLeft.x, posBottomRight.y}, {uvTopLeft.x, uvBottomRight.y}},
    };

    u16 idx[] = {0, 1, 2, 2, 3, 0};

    SetMeshNumVertices(entity, 4);
    SetMeshNumIndices(entity, 6);

    Assert(StreamOpen(SpriteVertexBuffer, StreamMode_Write));
    StreamWrite(SpriteVertexBuffer, sizeof(SpriteVertex) * 4, vts);
    StreamClose(SpriteVertexBuffer);

    Assert(StreamOpen(SpriteIndexBuffer, StreamMode_Write));
    StreamWrite(SpriteIndexBuffer, sizeof(u16) * 6, idx);
    StreamClose(SpriteIndexBuffer);
}

static void InitializeSpriteRoot(Entity entity) {
    SpriteVertexDeclaration = CreateVertexDeclaration(entity, "VertexDeclaration");

    auto positionAttribute = CreateVertexAttribute(entity, "Position");
    SetVertexAttributeUsage(positionAttribute, VertexAttributeUsage_Position);
    SetVertexAttributeType(positionAttribute, TypeOf_v2f());

    auto uvAttribute = CreateVertexAttribute(entity, "TexCoord");
    SetVertexAttributeUsage(uvAttribute, VertexAttributeUsage_TexCoord0);
    SetVertexAttributeType(uvAttribute, TypeOf_v2f());

    auto shaderDeclaration = CreateStream(entity, "ShaderDeclaration");
    SetStreamPath(shaderDeclaration, "res://scene2d/shaders/sprite.var");

    SpriteVertexShader = CreateShader(entity, "VertexShader");
    SetStreamPath(SpriteVertexShader, "res://scene2d/shaders/sprite.vs");
    SetShaderType(SpriteVertexShader, ShaderType_Vertex);
    SetShaderDeclaration(SpriteVertexShader, shaderDeclaration);

    SpritePixelShader = CreateShader(entity, "PixelShader");
    SetStreamPath(SpritePixelShader, "res://scene2d/shaders/sprite.ps");
    SetShaderType(SpritePixelShader, ShaderType_Pixel);
    SetShaderDeclaration(SpritePixelShader, shaderDeclaration);

    SpriteTextureUniform = CreateUniform(entity, "TextureUniform");
    SetUniformName(SpriteTextureUniform, "s_tex");
    SetUniformType(SpriteTextureUniform, TypeOf_Entity());
    SetUniformArrayCount(SpriteTextureUniform, 1);

    CreateSpriteMesh(entity);
}
DefineService(Sprite)
        ServiceEntity(SpriteRoot, InitializeSpriteRoot)
EndService()
