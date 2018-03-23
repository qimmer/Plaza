//
// Created by Kim Johannsen on 07-03-2018.
//

#include "SubTexture2D.h"
#include "Uniform.h"

struct SubTexture2D {
    v2i SubTexture2DOffset, SubTexture2DSize;
    Entity SubTexture2DTexture;
};

DefineComponent(SubTexture2D)
    DefineProperty(v2i, SubTexture2DOffset)
    DefineProperty(v2i, SubTexture2DSize)
    DefineProperty(Entity, SubTexture2DTexture)
EndComponent()

DefineService(SubTexture2D)
EndService()

DefineComponentProperty(SubTexture2D, v2i, SubTexture2DOffset)
DefineComponentProperty(SubTexture2D, v2i, SubTexture2DSize)
DefineComponentProperty(SubTexture2D, Entity, SubTexture2DTexture)

static Entity UvOffsetScaleUniform;

Entity GetSubTexture2DUvOffsetScaleUniform() {
    return UvOffsetScaleUniform;
}

static bool ServiceStart() {
    UvOffsetScaleUniform = CreateEntityFromPath("/.rendering/uniforms/uvOffsetScale");
    SetUniformName(UvOffsetScaleUniform, "uvOffsetScale");
    SetUniformType(UvOffsetScaleUniform, TypeOf_v4f());

    return true;
}

static bool ServiceStop() {
    DestroyEntity(UvOffsetScaleUniform);
    return true;
}