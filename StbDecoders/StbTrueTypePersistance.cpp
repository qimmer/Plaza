//
// Created by Kim on 19-10-2018.
//

#include "StbTrueTypePersistance.h"
#include <Foundation/NativeUtils.h>
#include <Foundation/AppLoop.h>
#include <Gui/Font.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/RenderContext.h>
#include <Core/Algorithms.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

#include "stb_truetype.h"

static eastl::set<Entity> invalidatedFonts;

static bool Serialize(Entity texture) {
    return false;
}

static bool Deserialize(Entity texture) {
    // Read and parse TTF font
    if(!StreamOpen(texture, StreamMode_Read)) {
        Error(texture, "Could not open font for reading: %s", GetStreamResolvedPath(texture));
        return false;
    }

    AddComponent(texture, ComponentOf_TrueTypeFont());

    StreamSeek(texture, StreamSeek_End);
    auto fontDataSize = StreamTell(texture);
    StreamSeek(texture, 0);

    auto fontData = (u8*)malloc(fontDataSize);
    StreamRead(texture, fontDataSize, fontData);
    StreamClose(texture);

    auto pixel_height = Max(GetTrueTypeFontSize(texture), 4);

    float scale;
    int x,y,bottom_y, i;
    stbtt_fontinfo f;
    f.userdata = NULL;
    if (!stbtt_InitFont(&f, fontData, 0))
        return false;

    u32 numGlyphs = 0;
    for_children(fontRange, TrueTypeFontRanges, texture) {
        auto unicodeRange = GetTrueTypeUnicodeRange(fontRange);
        numGlyphs += GetUnicodeRangeEnd(unicodeRange) - GetUnicodeRangeStart(unicodeRange);
    }

    SetNumFontGlyphs(texture, numGlyphs);
    auto& glyphs = GetFontGlyphs(texture);

    auto glyphIndex = 0;
    for_children(fontRange2, TrueTypeFontRanges, texture) {
        auto unicodeRange = GetTrueTypeUnicodeRange(fontRange2);

        for(auto unicode = GetUnicodeRangeStart(unicodeRange); unicode < GetUnicodeRangeEnd(unicodeRange); ++unicode) {
            SetGlyphCode(glyphs[glyphIndex], unicode);
            glyphIndex++;
        }
    }

    scale = stbtt_ScaleForPixelHeight(&f, pixel_height);

    // Calculate texture size
    auto isBigEnough = false;
    auto textureSize = 32;

    while(!isBigEnough) {
        x=y=1;
        bottom_y = 1;
        isBigEnough = true;
        textureSize *= 2;
        for (i=0; i < numGlyphs; ++i) {
            int advance, lsb, x0,y0,x1,y1,gw,gh;
            auto glyphData = GetGlyphData(glyphs[i]);

            auto glyphIndex = stbtt_FindGlyphIndex(&f, glyphData->GlyphCode);

            stbtt_GetGlyphHMetrics(&f, glyphIndex, &advance, &lsb);
            stbtt_GetGlyphBitmapBox(&f, glyphIndex, scale,scale, &x0,&y0,&x1,&y1);
            gw = x1-x0;
            gh = y1-y0;

            if (y+gh+1 > bottom_y)
                bottom_y = y+gh+1;

            x = x + gw + 1;

            if (x + gw + 1 >= textureSize)
                y = bottom_y, x = 1; // advance to next row

            if (y + gh + 1 >= textureSize) {
                isBigEnough = false;
                break;
            }
        }
    }

    x=y=1;
    bottom_y = 1;

    for (i=0; i < numGlyphs; ++i) {
        int advance, lsb, x0,y0,x1,y1,gw,gh;
        auto glyphData = GetGlyphData(glyphs[i]);
        auto glyphIndex = stbtt_FindGlyphIndex(&f, glyphData->GlyphCode);

        stbtt_GetGlyphHMetrics(&f, glyphIndex, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&f, glyphIndex, scale,scale, &x0,&y0,&x1,&y1);
        gw = x1-x0;
        gh = y1-y0;

        SetGlyphStartUv(glyphs[i], {(float)x / textureSize, (float)y / textureSize});
        SetGlyphEndUv(glyphs[i], {(float)(x + gw) / textureSize, (float)(y + gh) / textureSize});
        SetGlyphAdvance(glyphs[i], scale * advance);
        SetGlyphOffset(glyphs[i], {x0, y0});

        if (y+gh+1 > bottom_y)
            bottom_y = y+gh+1;

        x = x + gw + 1;

        if (x + gw + 1 >= textureSize)
            y = bottom_y, x = 1; // advance to next row
    }

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&f, &ascent, &descent, &lineGap);

    free(fontData);

    SetFontAscent(texture, ascent * scale);
    SetFontDescent(texture, descent * scale);
    SetFontLineGap(texture, lineGap * scale);

    SetTextureFormat(texture, TextureFormat_A8);
    SetTextureFlag(texture, TextureFlag_MIN_POINT | TextureFlag_MAG_POINT);
    SetTextureSize2D(texture, {textureSize, textureSize});

    return true;
}

static bool Compress(Entity entity, u64 offset, u64 size, const void *pixels) {
    return false;
}

static bool Decompress(Entity entity, u64 offset, u64 size, void *pixels) {
    Assert(entity, offset == 0);

    u8 *pixelsA = (u8*)pixels; // Font textures are always TextureFormat_A8
    memset(pixelsA + offset, 0, size);

    // Read and parse TTF font
    if(!StreamOpen(entity, StreamMode_Read)) {
        Error(entity, "Could not open font for reading: %s", GetStreamResolvedPath(entity));
        return false;
    }

    auto textureSize = GetTextureSize2D(entity);
    if(textureSize.x <= 0 || textureSize.y <= 0) {
        return false;
    }

    StreamSeek(entity, StreamSeek_End);
    auto fontDataSize = StreamTell(entity);
    StreamSeek(entity, 0);

    auto fontData = (u8*)malloc(fontDataSize);
    StreamRead(entity, fontDataSize, fontData);
    StreamClose(entity);

    auto& glyphs = GetFontGlyphs(entity);

    auto pixel_height = GetTrueTypeFontSize(entity);
    {
        float scale;
        int x,y,bottom_y, i;
        stbtt_fontinfo f;
        f.userdata = NULL;
        if (!stbtt_InitFont(&f, fontData, 0))
            return false;

        x=y=1;
        bottom_y = 1;

        scale = stbtt_ScaleForPixelHeight(&f, pixel_height);

        for (i=0; i < glyphs.size(); ++i) {
            int advance, lsb, x0,y0,x1,y1,gw,gh;
            auto glyphData = GetGlyphData(glyphs[i]);
            auto glyphIndex = stbtt_FindGlyphIndex(&f, glyphData->GlyphCode);

            stbtt_GetGlyphHMetrics(&f, glyphIndex, &advance, &lsb);
            stbtt_GetGlyphBitmapBox(&f, glyphIndex, scale,scale, &x0,&y0,&x1,&y1);
            gw = x1-x0;
            gh = y1-y0;
            if (x + gw + 1 >= textureSize.x)
                y = bottom_y, x = 1; // advance to next row
            if (y + gh + 1 >= textureSize.y) {
                Warning(entity, "Font texture size is not big enough to fit all glyphs. %d glyphs rendered, %d are still missing.", i, glyphs.size() - i);
                return true;
            }

            Assert(entity, x+gw < textureSize.x);
            Assert(entity, y+gh < textureSize.y);
            stbtt_MakeGlyphBitmap(&f, pixelsA+x+y*textureSize.x, gw,gh,textureSize.y, scale,scale, glyphIndex);

            x = x + gw + 1;
            if (y+gh+1 > bottom_y)
                bottom_y = y+gh+1;
        }
    }

    free(fontData);

    return true;
}

LocalFunction(ReloadFont, void, Entity font) {
	Deserialize(font);
}

LocalFunction(OnRangeChanged, void, Entity entity) {
    invalidatedFonts.insert(GetOwnership(entity).Owner);
}

LocalFunction(OnFontChanged, void, Entity entity) {
    invalidatedFonts.insert(entity);
}
LocalFunction(OnUnicodeRangeChanged, void, Entity entity) {
    for_entity(range, ComponentOf_TrueTypeFontRange()) {
        if(data->TrueTypeUnicodeRange == entity) {
            OnRangeChanged(range);
        }
    }
}

LocalFunction(OnFontValidation, void) {
    for(auto& font : invalidatedFonts) {
        ReloadFont(font);
    }

    invalidatedFonts.clear();
}

BeginUnit(StbTrueTypePersistance)
    BeginComponent(UnicodeRange)
        RegisterProperty(u16, UnicodeRangeStart)
        RegisterProperty(u16, UnicodeRangeEnd)
    EndComponent()

    BeginComponent(TrueTypeFontRange)
        RegisterReferenceProperty(UnicodeRange, TrueTypeUnicodeRange)
    EndComponent()

    BeginComponent(TrueTypeFont)
        RegisterProperty(float, TrueTypeFontSize)
        RegisterArrayProperty(TrueTypeFontRange, TrueTypeFontRanges)
    EndComponent()

    RegisterStreamCompressor(ComponentOf_Font(), "font/ttf")
    RegisterSerializer(TTF, "font/ttf")

	RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UnicodeRangeStart()), OnUnicodeRangeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UnicodeRangeEnd()), OnUnicodeRangeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TrueTypeUnicodeRange()), OnRangeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TrueTypeFontSize()), OnFontChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnFontValidation, AppLoopOf_ResourcePrePreparation())
EndUnit()
