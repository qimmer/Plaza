//
// Created by Kim Johannsen on 04-04-2018.
//

#include <Foundation/Stream.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/Material.h>
#include <Rendering/UniformState.h>
#include <Rendering/Uniform.h>
#include <Rendering/Shader.h>
#include <Rendering/VertexDeclaration.h>
#include <Rendering/VertexAttribute.h>
#include "Font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

#include "stb_truetype.h"

Entity FontTextureUniform = 0, FontRoot = 0, FontVertexShader = 0, FontPixelShader = 0, FontVertexDeclaration = 0;

struct Font {
    Font() : FontFirstChar(32), FontLastChar(96+32), FontSize(12.0f), Invalidated(true) {}

    Entity Texture, Material;
    Vector<stbtt_bakedchar> Characters;
    u32 FontFirstChar, FontLastChar;
    float FontSize;
    bool Invalidated;
};

#define FONT_TEXTURE_DIM 512

static u8 textureData[FONT_TEXTURE_DIM*FONT_TEXTURE_DIM];

BeginUnit(Font)
    BeginComponent(Font)
    RegisterBase(Stream)
    RegisterProperty(float, FontSize))
    RegisterProperty(u32, FontFirstChar))
    RegisterProperty(u32, FontLastChar))
EndComponent()

RegisterProperty(float, FontSize)
RegisterProperty(u32, FontFirstChar)
RegisterProperty(u32, FontLastChar)

static void BakeFont(Entity entity) {
    if(!HasComponent(entity, ComponentOf_Font())) return;

    auto data = GetFontData(entity);
    data->Characters.resize(data->FontLastChar - data->FontFirstChar);

    // Read and parse TTF font
    if(!StreamOpen(entity, StreamMode_Read)) {
        Log(LogChannel_Core, LogSeverity_Error, "Could not open font for reading: %s", GetStreamResolvedPath(entity));
        return;
    }

    StreamSeek(entity, StreamSeek_End);
    auto size = StreamTell(entity);
    StreamSeek(entity, 0);

    auto buf = malloc(size);
    StreamRead(entity, size, buf);
    stbtt_BakeFontBitmap((const u8*)buf,0, GetFontSize(entity), textureData,FONT_TEXTURE_DIM,FONT_TEXTURE_DIM, data->FontFirstChar, data->FontLastChar - data->FontFirstChar, data->Characters.data());
    free(buf);

    StreamClose(entity);

    // Write out new texture data
    SetTextureSize2D(data->Texture, {FONT_TEXTURE_DIM, FONT_TEXTURE_DIM});
    SetTextureFormat(data->Texture, TextureFormat_A8);
    SetTextureFlag(data->Texture, TextureFlag_MIN_POINT | TextureFlag_MAG_POINT);
    if(StreamOpen(data->Texture, StreamMode_Write)) {
        StreamWrite(data->Texture, FONT_TEXTURE_DIM * FONT_TEXTURE_DIM, textureData);
        StreamClose(data->Texture);
    }
}

static int intFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
    unsigned int c = (unsigned int)-1;
    const unsigned char* str = (const unsigned char*)in_text;
    if (!(*str & 0x80))
    {
        c = (unsigned int)(*str++);
        *out_char = c;
        return 1;
    }
    if ((*str & 0xe0) == 0xc0)
    {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 2) return 1;
        if (*str < 0xc2) return 2;
        c = (unsigned int)((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80) return 2;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0)
    {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 3) return 1;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
        if (*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0)
    {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 4) return 1;
        if (*str > 0xf4) return 4;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
        if (*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 6);
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

u32 GetFontGlyphData(Entity font,
                      StringRef text,
                      v2f origin,
                      FontVertex *vertices,
                        u32 maxVertices) {
    auto data = GetFont(font);

    if(data->Invalidated) {
        data->Invalidated = false;

        BakeFont(font);
    }

    auto end = text + strlen(text);
    u32 numVertices = 0;

    while(text < end) {
        Assert((numVertices + 6) <= maxVertices);
        u32 ch;
        text += intFromUtf8(&ch, text, end);

        FontVertex vt[2];
        stbtt_GetBakedQuad(data->Characters.data(), FONT_TEXTURE_DIM,FONT_TEXTURE_DIM, ch-data->FontFirstChar, &origin.x,&origin.y,(stbtt_aligned_quad*)vt,1);//1=opengl & d3d10+,0=d3d9
        vertices[0] = {
            { vt[0].Position.x, vt[0].Position.y },
            { vt[0].Uv.x,       vt[0].Uv.y },
        };

        vertices[1] = {
            { vt[1].Position.x, vt[0].Position.y },
            { vt[1].Uv.x,       vt[0].Uv.y },
        };

        vertices[2] = {
            { vt[1].Position.x, vt[1].Position.y },
            { vt[1].Uv.x,       vt[1].Uv.y },
        };



        vertices[3] = {
            { vt[1].Position.x, vt[1].Position.y },
            { vt[1].Uv.x,       vt[1].Uv.y },
        };

        vertices[4] = {
            { vt[0].Position.x, vt[1].Position.y },
            { vt[0].Uv.x,       vt[1].Uv.y },
        };

        vertices[5] = {
            { vt[0].Position.x, vt[0].Position.y },
            { vt[0].Uv.x,       vt[0].Uv.y },
        };

        numVertices += 6;
        vertices += 6;
    }

    return numVertices;
}

Entity GetFontMaterial(Entity font) {
    return GetFont(font)->Material;
}

static void InvalidateFont(Entity entity) {
    GetFontData(entity)->Invalidated = true;
}


static void ChangeFont(Entity entity) {
    if(HasComponent(entity, ComponentOf_Font())) {
        FireEvent(EventOf_FontChanged(), entity);
    }
}

LocalFunction(OnFontAdded, void, Entity entity) {
    auto data = GetFontData(entity);
    data->Texture = CreateTexture2D(entity, "Texture");
    data->Material = CreateMaterial(entity, "Material");
    SetMaterialVertexShader(data->Material, FontVertexShader);
    SetMaterialPixelShader(data->Material, FontPixelShader);
    SetMaterialBlendMode(data->Material, RenderState_STATE_BLEND_ALPHA);

    auto textureState = CreateUniformState(data->Material, "TextureState");
    SetUniformStateTexture(textureState, data->Texture);
    SetUniformStateUniform(textureState, FontTextureUniform);
}

LocalFunction(OnFontStarted, void, Service service) {
    FontRoot = CreateNode(0, ".font");
    FontTextureUniform = CreateUniform(FontRoot, "TextureUniform");
    SetUniformName(FontTextureUniform, "s_tex");
    SetUniformType(FontTextureUniform, TypeOf_Entity());

    auto shaderDeclaration = CreateStream(FontRoot, "ShaderDeclaration");
    SetStreamPath(shaderDeclaration, "res://Assets/Shaders/font.var");

    FontVertexShader = CreateShader(FontRoot, "VertexShader");
    SetShaderType(FontVertexShader, ShaderType_Vertex);
    SetStreamPath(FontVertexShader, "res://Assets/Shaders/font.vs");
    SetShaderDeclaration(FontVertexShader, shaderDeclaration);

    FontPixelShader = CreateShader(FontRoot, "PixelShader");
    SetShaderType(FontPixelShader, ShaderType_Pixel);
    SetStreamPath(FontPixelShader, "res://Assets/Shaders/font.ps");
    SetShaderDeclaration(FontPixelShader, shaderDeclaration);

    FontVertexDeclaration = CreateVertexDeclaration(FontRoot, "VertexDeclaration");
    auto positionAttribute = CreateVertexAttribute(FontVertexDeclaration, "Position");
    SetVertexAttributeUsage(positionAttribute, VertexAttributeUsage_Position);
    SetVertexAttributeType(positionAttribute, TypeOf_v2f());

    auto uvAttribute = CreateVertexAttribute(FontVertexDeclaration, "TexCoord");
    SetVertexAttributeUsage(uvAttribute, VertexAttributeUsage_TexCoord0);
    SetVertexAttributeType(uvAttribute, TypeOf_v2f());

}

LocalFunction(OnFontStopped, void, Service service) {
    DestroyEntity(FontRoot);
}

DefineService(Font)
    RegisterSubscription(FontChanged, InvalidateFont, 0)
    RegisterSubscription(StreamChanged, ChangeFont, 0)
    RegisterSubscription(StreamContentChanged, ChangeFont, 0)
    RegisterSubscription(FontAdded, OnFontAdded, 0)
    RegisterSubscription(FontStarted, OnFontStarted, 0)
    RegisterSubscription(FontStopped, OnFontStopped, 0)
EndService()

Entity GetFontVertexDeclaration() {
    return FontVertexDeclaration;
}
