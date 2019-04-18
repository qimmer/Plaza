//
// Created by Kim Johannsen on 04-04-2018.
//

#ifndef PLAZA_FONT_H
#define PLAZA_FONT_H

#include <Core/NativeUtils.h>

struct Glyph {
    u32 GlyphCode;
    v2f GlyphStartUv, GlyphEndUv;
    v2i GlyphOffset;
    float GlyphAdvance;
};

struct Font {
    ChildArray FontGlyphs;
    s32 FontAscent, FontDescent, FontLineGap;
};

struct FontVertex {
    v2f Position, Uv;
    v4f Color;
};

u32 GetFontGlyphData(Entity font,
                     StringRef text,
                     const v4f *colors,
                     v2f origin,
                     v2f *size,
                     FontVertex *vertices,
                     u32 maxVertices);

Unit(Font)
    Component(Glyph)
        Property(u32, GlyphCode)
        Property(v2f, GlyphStartUv)
        Property(v2f, GlyphEndUv)
        Property(v2i, GlyphOffset)
        Property(float, GlyphAdvance)

    Component(Font)
        Property(s32, FontAscent)
        Property(s32, FontDescent)
        Property(s32, FontLineGap)
        ArrayProperty(Glyph, FontGlyphs)

#endif //PLAZA_FONT_H
