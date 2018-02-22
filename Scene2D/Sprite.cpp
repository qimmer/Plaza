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
#include "Sprite.h"

    struct Sprite {
        v2f SpriteUvOffset, SpriteUvSize;
    };

    struct SpriteVertex {
        v2f Position, Uv;
    };

    static Entity SpriteVertexDeclaration, SpriteProgram;

    Entity SpriteRoot;

    DefineComponent(Sprite)
        Dependency(Mesh)
    EndComponent()

    DefineService(Sprite)
    EndService()

    DefineComponentProperty(Sprite, v2f, SpriteUvOffset)
    DefineComponentProperty(Sprite, v2f, SpriteUvSize)

    static void OnUpdateMesh(Entity entity, v2f oldValue, v2f newValue) {
        v2f offset = GetSpriteUvOffset(entity);
        v2f size = GetSpriteUvSize(entity);
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
        auto VertexShader = CreateShader(FormatString("%s/VertexShader", GetEntityPath(SpriteRoot)));
        auto PixelShader = CreateShader(FormatString("%s/PixelShader", GetEntityPath(SpriteRoot)));

        auto VertexShaderStream = CreateFileStream(FormatString("%s/Stream", GetEntityPath(VertexShader)));
        auto PixelShaderStream = CreateFileStream(FormatString("%s/Stream", GetEntityPath(PixelShader)));
        auto VaryingDefStream = CreateFileStream(FormatString("%s/Stream", GetEntityPath(SpriteProgram)));

        SetFilePath(VertexShaderStream, "Shaders/sprite.vs");
        SetFilePath(PixelShaderStream, "Shaders/sprite.ps");
        SetFilePath(VaryingDefStream, "Shaders/sprite.var");

        SetShaderSourceStream(VertexShader, VertexShaderStream);
        SetShaderSourceStream(PixelShader, PixelShaderStream);
        SetShaderVaryingDefStream(VertexShader, VaryingDefStream);
        SetShaderVaryingDefStream(PixelShader, VaryingDefStream);

        SetShaderType(VertexShader, ShaderType_Vertex);
        SetShaderType(PixelShader, ShaderType_Pixel);

        SetVertexShader(SpriteProgram, VertexShader);
        SetPixelShader(SpriteProgram, PixelShader);
    }

    static bool ServiceStart() {
        SubscribeSpriteUvOffsetChanged(OnUpdateMesh);
        SubscribeSpriteUvSizeChanged(OnUpdateMesh);

        SpriteRoot = CreateHierarchy("/.Sprite");

        InitializeSpriteProgram();
        InitializeSpriteVertexDeclaration();

        return true;
    }

    static bool ServiceStop() {
        DestroyEntity(SpriteRoot);

        UnsubscribeSpriteUvOffsetChanged(OnUpdateMesh);
        UnsubscribeSpriteUvSizeChanged(OnUpdateMesh);

        return true;
    }
