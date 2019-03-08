//
// Created by Kim Johannsen on 04-04-2018.
//

#include <Foundation/Stream.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/Material.h>
#include <Rendering/Uniform.h>
#include <Rendering/Mesh.h>
#include <Rendering/Renderable.h>
#include <Core/Debug.h>
#include <Core/Algorithms.h>
#include <Core/Identification.h>
#include "Font.h"
#include "Widget.h"

typedef struct {
    float x0, y0, s0, t0; // top-left
    float x1, y1, s1, t1; // bottom-right
} AlignedQuad;

static int intFromUtf8(unsigned int *out_char, const char *in_text, const char *in_text_end) {
    unsigned int c = (unsigned int) -1;
    const unsigned char *str = (const unsigned char *) in_text;
    if (!(*str & 0x80)) {
        c = (unsigned int) (*str++);
        *out_char = c;
        return 1;
    }
    if ((*str & 0xe0) == 0xc0) {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char *) str < 2) return 1;
        if (*str < 0xc2) return 2;
        c = (unsigned int) ((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80) return 2;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0) {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char *) str < 3) return 1;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
        if (*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
        c = (unsigned int) ((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (unsigned int) ((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0) {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char *) str < 4) return 1;
        if (*str > 0xf4) return 4;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
        if (*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
        c = (unsigned int) ((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int) ((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int) ((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (*str++ & 0x3f);
        // utf-8 encodings of values used in surrogate pairs are invalid
        if ((c & 0xFFFFF800) == 0xD800) return 4;
        *out_char = c;
        return 4;
    }
    *out_char = 0;
    return 0;
}

inline void GetBakedQuad(const Glyph *b, int pw, int ph, float *xpos, float *ypos, v4f color, FontVertex *vertices) {
    float ipw = 1.0f / pw, iph = 1.0f / ph;
    int round_x = (int) floor((*xpos + b->GlyphOffset.x));
    int round_y = (int) floor((*ypos + b->GlyphOffset.y));

    auto x0 = (float)round_x;
    auto y0 = (float)round_y;
    auto x1 = (float)round_x + (b->GlyphEndUv.x * pw) - (b->GlyphStartUv.x * pw);
    auto y1 = (float)round_y + (b->GlyphEndUv.y * ph) - (b->GlyphStartUv.y * ph);

    auto s0 = b->GlyphStartUv.x;
    auto t0 = b->GlyphStartUv.y;
    auto s1 = b->GlyphEndUv.x;
    auto t1 = b->GlyphEndUv.y;

    *xpos += b->GlyphAdvance;

    vertices[0] = {
            {x0, y0},
            {s0, t0},
            color
    };

    vertices[1] = {
            {x1, y0},
            {s1, t0},
            color
    };

    vertices[2] = {
            {x1, y1},
            {s1, t1},
            color
    };


    vertices[3] = {
            {x1, y1},
            {s1, t1},
            color
    };

    vertices[4] = {
            {x0, y1},
            {s0, t1},
            color
    };

    vertices[5] = {
            {x0, y0},
            {s0, t0},
            color
    };
}

API_EXPORT u32 GetFontGlyphData(Entity font,
                            StringRef text,
                            const v4f *colors,
                            v2f origin,
                            v2f *size,
                            FontVertex *vertices,
                            u32 maxVertices) {
    auto data = GetFontData(font);
    auto textureSize = GetTextureSize2D(font);

    auto end = text + (text ? strlen(text) : 0);
    u32 numVertices = 0;

    *size = {0.0f, 0.0f};

    origin.y += data->FontAscent;

    auto i = 0;

	auto& fontGlyphs = GetFontGlyphs(font);
	auto glyphsData = (Glyph*)alloca(sizeof(Glyph) * fontGlyphs.size());
	for (auto j = 0; j < fontGlyphs.size(); ++j) {
		glyphsData[j] = *GetGlyphData(fontGlyphs[j]);
	}

    while (text < end) {
        Assert(font, (numVertices + 6) <= maxVertices);
        u32 ch = 0;
        text += intFromUtf8(&ch, text, end);

        if(ch == '\n') {
            origin.x = 0;
            origin.y += -data->FontDescent + data->FontLineGap + data->FontAscent;
            continue;
        }

		Glyph *glyphData = NULL;
		for (auto j = 0; j < fontGlyphs.size(); ++j) {
			if (glyphsData[j].GlyphCode == ch) {
				glyphData = &glyphsData[j];
				break;
			}
		}
        
        if (glyphData) {
            GetBakedQuad(glyphData, textureSize.x, textureSize.y, &origin.x, &origin.y, colors[i], vertices);//1=opengl & d3d10+,0=d3d9

            numVertices += 6;
            vertices += 6;
        }

        size->x = Max(size->x, origin.x);

        ++i;
    }

    if(size) {
        size->y = origin.y - data->FontDescent;
    }

    return numVertices;
}

BeginUnit(Font)
    BeginComponent(Glyph)
        RegisterProperty(u32, GlyphCode)
        RegisterProperty(v2f, GlyphStartUv)
        RegisterProperty(v2f, GlyphEndUv)
        RegisterProperty(v2i, GlyphOffset)
        RegisterProperty(float, GlyphAdvance)
    EndComponent()

    BeginComponent(Font)
        RegisterBase(Texture2D)
        RegisterArrayProperty(Glyph, FontGlyphs)
        RegisterProperty(s32, FontAscent)
        RegisterProperty(s32, FontDescent)
        RegisterProperty(s32, FontLineGap)
    EndComponent()
EndUnit()
