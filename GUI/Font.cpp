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
#include <Foundation/Invalidation.h>
#include <Core/Algorithms.h>
#include <Core/Identification.h>
#include "Font.h"

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

static void GetBakedQuad(const Glyph *b, int pw, int ph, float *xpos, float *ypos, FontVertex *vertices, bool opengl_fillrule) {
    float d3d_bias = opengl_fillrule ? 0 : -0.5f;
    float ipw = 1.0f / pw, iph = 1.0f / ph;
    int round_x = (int) floor((*xpos + b->GlyphOffset.x) + 0.5f);
    int round_y = (int) floor((*ypos + b->GlyphOffset.y) + 0.5f);

    auto x0 = round_x + d3d_bias;
    auto y0 = round_y + d3d_bias;
    auto x1 = round_x + (b->GlyphEndUv.x * pw) - (b->GlyphStartUv.x * pw) + d3d_bias;
    auto y1 = round_y + (b->GlyphEndUv.y * ph) - (b->GlyphStartUv.y * ph) + d3d_bias;

    auto s0 = (b->GlyphStartUv.x * pw) * ipw;
    auto t0 = (b->GlyphStartUv.y * ph) * iph;
    auto s1 = (b->GlyphEndUv.x * pw) * ipw;
    auto t1 = (b->GlyphEndUv.y * ph) * iph;

    *xpos += b->GlyphAdvance;

    vertices[0] = {
            {x0, y0},
            {s0, t0},
    };

    vertices[1] = {
            {x1, y0},
            {s1, t0},
    };

    vertices[2] = {
            {x1, y1},
            {s1, t1},
    };


    vertices[3] = {
            {x1, y1},
            {s1, t1},
    };

    vertices[4] = {
            {x0, y1},
            {s0, t1},
    };

    vertices[5] = {
            {x0, y0},
            {s0, t0},
    };
}

static u32 GetFontGlyphData(Entity font,
                            StringRef text,
                            v2f origin,
                            FontVertex *vertices,
                            u32 maxVertices) {
    auto data = GetFontData(font);
    auto textureSize = GetTextureSize2D(font);

    auto end = text + strlen(text);
    u32 numVertices = 0;

    while (text < end) {
        Assert(font, (numVertices + 6) <= maxVertices);
        u32 ch = 0;
        text += intFromUtf8(&ch, text, end);

        Glyph *glyphData = NULL;
        for_children(glyph, FontGlyphs, font, {
            glyphData = GetGlyphData(glyph);
            if (glyphData->GlyphCode == ch) {
                break;
            }
            glyphData = NULL;
        });

        if (glyphData) {
            GetBakedQuad(glyphData, textureSize.x, textureSize.y, &origin.x, &origin.y, vertices, 1);//1=opengl & d3d10+,0=d3d9

            numVertices += 6;
            vertices += 6;
        }
    }

    return numVertices;
}

static void RebuildTextMesh(Entity entity) {
    if (!HasComponent(entity, ComponentOf_TextMesh())) return;

    auto data = GetTextMeshData(entity);

    if (IsEntityValid(data->TextMeshFont)) {
        auto length = strlen(data->TextMeshText);
        auto vertices = (FontVertex *) malloc(sizeof(FontVertex) * length * 6);
        auto numVertices = GetFontGlyphData(data->TextMeshFont, data->TextMeshText, {0.0f, 0.0f}, vertices,
                                            length * 6);

        v2f min = {FLT_MAX, FLT_MAX}, max = {FLT_MIN, FLT_MIN};
        for (auto i = 0; i < numVertices; ++i) {
            min.x = Min(min.x, vertices[i].Position.x);
            min.y = Min(min.y, vertices[i].Position.y);

            max.x = Max(max.x, vertices[i].Position.x);
            max.y = Max(max.y, vertices[i].Position.y);
        }

        auto vertexBuffer = GetMeshVertexBuffer(entity);
        SetVertexBufferDeclaration(vertexBuffer, FindEntityByUuid("Gui.Font.VertexDeclaration"));

        auto currentPath = GetStreamPath(vertexBuffer);
        if(!currentPath || !strlen(currentPath)) {
            char path[1024];
            snprintf(path, sizeof(path), "memory://%s", GetUuid(vertexBuffer));
            SetStreamPath(vertexBuffer, path);
        }

        if(!StreamOpen(vertexBuffer, StreamMode_Write)) {
            free(vertices);
            return;
        }

        StreamWrite(vertexBuffer, numVertices * sizeof(FontVertex), vertices);
        StreamClose(vertexBuffer);

        SetNumMeshSubMeshes(entity, 1);
        auto subMesh = *GetMeshSubMeshes(entity, NULL);

        SetSubMeshNumVertices(subMesh, numVertices);
        SetSubMeshPrimitiveType(subMesh, PrimitiveType_TRIANGLELIST);
        free(vertices);
    }
}

LocalFunction(OnValidateMeshes, void, Entity component) {
    for_entity(textMesh, data, TextMesh, {
        if(IsDirty(textMesh)) {
            RebuildTextMesh(textMesh);
        }
    });
}

LocalFunction(OnValidateTextures, void, Entity component) {
    for_entity(font, data, Font, {
        if(IsDirty(font)) {
            for_entity(textMesh, data, TextMesh, {
                if(data->TextMeshFont == font) {
                    RebuildTextMesh(textMesh);
                }
            });
        }
    });
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
    EndComponent()

    BeginComponent(TextMesh)
        RegisterProperty(Entity, TextMeshFont)
        RegisterProperty(StringRef, TextMeshText)
        RegisterProperty(v2f, TextMeshAlignment)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextMeshFont()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextMeshText()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FontGlyphs()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_GlyphCode()), InvalidateParent, 0)
    RegisterSubscription(EventOf_Validate(), OnValidateMeshes, ComponentOf_Mesh())
    RegisterSubscription(EventOf_Validate(), OnValidateTextures, ComponentOf_Texture())
EndUnit()
