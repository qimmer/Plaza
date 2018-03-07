//
// Created by Kim Johannsen on 07-03-2018.
//

#include "SubTexture2D.h"
#include "Uniform.h"

struct SubTexture2D {
    v2i SubTexture2DOffset, SubTexture2DSize;
};

DefineComponent(SubTexture2D)
EndComponent()

DefineService(SubTexture2D)
EndService()

DefineComponentProperty(SubTexture2D, v2i, SubTexture2DOffset)
DefineComponentProperty(SubTexture2D, v2i, SubTexture2DSize)

static Entity UvOffsetScaleUniform;

Entity GetSubTexture2DUvOffsetScaleUniform() {
    return UvOffsetScaleUniform;
}

static bool ServiceStart() {
    UvOffsetScaleUniform = CreateUniform("/.rendering/uniforms/uvOffsetScale");
    SetUniformName(UvOffsetScaleUniform, "uvOffsetScale");
    SetUniformType(UvOffsetScaleUniform, TypeOf_v4f());

    return true;
}

static bool ServiceStop() {
    DestroyEntity(UvOffsetScaleUniform);
    return true;
}