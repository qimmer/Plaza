//
// Created by Kim Johannsen on 04-04-2018.
//

#ifndef PLAZA_FONT_H
#define PLAZA_FONT_H

#include <Core/Entity.h>

DeclareComponent(Font)
DeclareComponentPropertyReactive(Font, float, FontSize)
DeclareComponentPropertyReactive(Font, u32, FontFirstChar)
DeclareComponentPropertyReactive(Font, u32, FontLastChar)

DeclareService(Font)

struct FontVertex {
    v2f Position, Uv;
};

u32 GetFontGlyphData(Entity font, StringRef text, v2f origin, FontVertex *verticesOut, u32 maxVertices);
Entity GetFontMaterial(Entity font);
Entity GetFontVertexDeclaration();

#endif //PLAZA_FONT_H
