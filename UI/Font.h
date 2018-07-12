//
// Created by Kim Johannsen on 04-04-2018.
//

#ifndef PLAZA_FONT_H
#define PLAZA_FONT_H

#include <Core/NativeUtils.h>

Unit(Font)
    Component(Font)
        Property(float, FontSize)
        Property(u32, FontFirstChar)
        Property(u32, FontLastChar)


struct FontVertex {
    v2f Position, Uv;
};

u32 GetFontGlyphData(Entity font, StringRef text, v2f origin, FontVertex *verticesOut, u32 maxVertices);
Entity GetFontMaterial(Entity font);
Entity GetFontVertexDeclaration();

#endif //PLAZA_FONT_H
