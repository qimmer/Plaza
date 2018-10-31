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
    StringRef FontCharacters;
};

struct FontVertex {
    v2f Position, Uv;
};

struct Label {
    Entity LabelFont;
    StringRef LabelText;
    v2f LabelAlignment;
};

Unit(Font)
    Component(Glyph)
        Property(u32, GlyphCode)
        Property(v2f, GlyphStartUv)
        Property(v2f, GlyphEndUv)
        Property(v2i, GlyphOffset)
        Property(float, GlyphAdvance)

    Component(Font)
        Property(StringRef, FontCharacters)
        ArrayProperty(Glyph, FontGlyphs)

    Component(Label)
        ReferenceProperty(Font, LabelFont)
        Property(StringRef, LabelText)
        Property(v2f, LabelAlignment)

#endif //PLAZA_FONT_H
