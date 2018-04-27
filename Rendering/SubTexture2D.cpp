//
// Created by Kim Johannsen on 07-03-2018.
//

#include "SubTexture2D.h"
#include "Uniform.h"

Entity UvOffsetScaleUniform = 0;

struct SubTexture2D {
    v2i SubTexture2DOffset, SubTexture2DSize;
    Entity SubTexture2DTexture;
};

DefineComponent(SubTexture2D)
    DefinePropertyReactive(v2i, SubTexture2DOffset)
    DefinePropertyReactive(v2i, SubTexture2DSize)
    DefinePropertyReactive(Entity, SubTexture2DTexture)
EndComponent()

DefineComponentPropertyReactive(SubTexture2D, v2i, SubTexture2DOffset)
DefineComponentPropertyReactive(SubTexture2D, v2i, SubTexture2DSize)
DefineComponentPropertyReactive(SubTexture2D, Entity, SubTexture2DTexture)

Entity GetSubTexture2DUvOffsetScaleUniform() {
    return UvOffsetScaleUniform;
}

static void InitializeUvOffsetScaleUniform(Entity entity) {
    SetUniformName(entity, "uvOffsetScale");
    SetUniformType(entity, TypeOf_v4f());
}

DefineService(SubTexture2D)
    ServiceEntity(UvOffsetScaleUniform, "/.SubTexture2D/UvOffsetScaleUniform", InitializeUvOffsetScaleUniform)
EndService()
