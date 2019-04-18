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
        Error(texture, "Could not open font for reading: %s", GetStream(texture).StreamResolvedPath);
        return false;
    }

    auto trueTypeData = GetTrueTypeFont(texture);

    StreamSeek(texture, StreamSeek_End);
    auto fontDataSize = StreamTell(texture);
    StreamSeek(texture, 0);

    auto fontData = (u8*)malloc(fontDataSize);
    StreamRead(texture, fontDataSize, fontData);
    StreamClose(texture);

    auto pixel_height = Max(trueTypeData.TrueTypeFontSize, 4);

    float scale;
    int x,y,bottom_y, i;
    stbtt_fontinfo f;
    f.userdata = NULL;
    if (!stbtt_InitFont(&f, fontData, 0))
        return false;

    u32 numGlyphs = 0;
    for(auto fontRange : trueTypeData.TrueTypeFontRanges) {
        auto unicodeRange = GetUnicodeRange(GetTrueTypeFontRange(fontRange).TrueTypeUnicodeRange);
        numGlyphs += unicodeRange.UnicodeRangeEnd - unicodeRange.UnicodeRangeStart;
    }

    auto fontDetails = GetFont(texture);
    fontDetails.FontGlyphs.SetSize(numGlyphs);

    auto glyphIndex = 0;
    for(auto fontRange2 : trueTypeData.TrueTypeFontRanges) {
        auto unicodeRange = GetUnicodeRange(GetTrueTypeFontRange(fontRange2).TrueTypeUnicodeRange);

        for(auto unicode = unicodeRange.UnicodeRangeStart; unicode < unicodeRange.UnicodeRangeEnd; ++unicode) {
            if(!fontDetails.FontGlyphs[glyphIndex]) {
                fontDetails.FontGlyphs[glyphIndex] = 0;
            }
            auto glyphData = GetGlyph(fontDetails.FontGlyphs[glyphIndex]);
            glyphData.GlyphCode = unicode;
            SetGlyph(fontDetails.FontGlyphs[glyphIndex], glyphData);
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
            auto glyphData = GetGlyph(fontDetails.FontGlyphs[i]);

            auto glyphIndex = stbtt_FindGlyphIndex(&f, glyphData.GlyphCode);

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
        auto glyphData = GetGlyph(fontDetails.FontGlyphs[i]);
        auto glyphIndex = stbtt_FindGlyphIndex(&f, glyphData.GlyphCode);

        stbtt_GetGlyphHMetrics(&f, glyphIndex, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&f, glyphIndex, scale,scale, &x0,&y0,&x1,&y1);
        gw = x1-x0;
        gh = y1-y0;

        glyphData.GlyphStartUv = {(float)x / textureSize, (float)y / textureSize};
        glyphData.GlyphEndUv = {(float)(x + gw) / textureSize, (float)(y + gh) / textureSize};
        glyphData.GlyphAdvance = scale * advance;
        glyphData.GlyphOffset = {x0, y0};
        SetGlyph(fontDetails.FontGlyphs[i], glyphData);

        if (y+gh+1 > bottom_y)
            bottom_y = y+gh+1;

        x = x + gw + 1;

        if (x + gw + 1 >= textureSize)
            y = bottom_y, x = 1; // advance to next row
    }

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&f, &ascent, &descent, &lineGap);

    free(fontData);

    fontDetails.FontAscent = ascent * scale;
    fontDetails.FontDescent = descent * scale;
    fontDetails.FontLineGap = lineGap * scale;

    auto textureData = GetTexture(texture);
    textureData.TextureFormat = TextureFormat_A8;
    textureData.TextureFlag = TextureFlag_MIN_POINT | TextureFlag_MAG_POINT;

    SetTexture2D(texture, {textureSize, textureSize});
    SetTexture(texture, textureData);
    SetFont(texture, fontDetails);

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
        Error(entity, "Could not open font for reading: %s", GetStream(entity).StreamResolvedPath);
        return false;
    }

    auto textureSize = GetTexture2D(entity).TextureSize2D;
    if(textureSize.x <= 0 || textureSize.y <= 0) {
        return false;
    }

    StreamSeek(entity, StreamSeek_End);
    auto fontDataSize = StreamTell(entity);
    StreamSeek(entity, 0);

    auto fontData = (u8*)malloc(fontDataSize);
    StreamRead(entity, fontDataSize, fontData);
    StreamClose(entity);

    auto fontDetails = GetFont(entity);

    auto pixel_height = GetTrueTypeFont(entity).TrueTypeFontSize;
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

        for (i=0; i < fontDetails.FontGlyphs.GetSize(); ++i) {
            int advance, lsb, x0,y0,x1,y1,gw,gh;
            auto glyphData = GetGlyph(fontDetails.FontGlyphs[i]);
            auto glyphIndex = stbtt_FindGlyphIndex(&f, glyphData.GlyphCode);

            stbtt_GetGlyphHMetrics(&f, glyphIndex, &advance, &lsb);
            stbtt_GetGlyphBitmapBox(&f, glyphIndex, scale,scale, &x0,&y0,&x1,&y1);
            gw = x1-x0;
            gh = y1-y0;
            if (x + gw + 1 >= textureSize.x)
                y = bottom_y, x = 1; // advance to next row
            if (y + gh + 1 >= textureSize.y) {
                Warning(entity, "Font texture size is not big enough to fit all glyphs. %d glyphs rendered, %d are still missing.", i, fontDetails.FontGlyphs.GetSize() - i);
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

static void ReloadFont(Entity font) {
	Deserialize(font);
}

static void OnRangeChanged(Entity entity, const TrueTypeFontRange& oldData, const TrueTypeFontRange& newData) {
    invalidatedFonts.insert(GetOwnership(entity).Owner);
}

static void OnFontChanged(Entity entity) {
    invalidatedFonts.insert(entity);
}
static void OnUnicodeRangeChanged(Entity entity) {
    TrueTypeFontRange data;
    for_entity_data(range, ComponentOf_TrueTypeFontRange(), &data) {
        if(data.TrueTypeUnicodeRange == entity) {
            invalidatedFonts.insert(GetOwnership(range).Owner);
        }
    }
}

static void OnAppLoopChanged(Entity entity, const AppLoop& oldData, const AppLoop& newData) {
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

    RegisterStreamCompressor(TTF, "font/ttf")
    RegisterSerializer(TTF, "font/ttf")

    RegisterSystem(OnUnicodeRangeChanged, ComponentOf_UnicodeRange())
    RegisterSystem(OnFontChanged, ComponentOf_TrueTypeFont())
    RegisterSystem(OnRangeChanged, ComponentOf_TrueTypeFontRange())

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Rendering)
EndUnit()
