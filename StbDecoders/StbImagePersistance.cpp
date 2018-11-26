//
// Created by Kim Johannsen on 06-03-2018.
//

#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>
#include <memory.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Core/Debug.h>
#include <Foundation/NativeUtils.h>
#include "StbImagePersistance.h"

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static bool Serialize(Entity texture) {
    return false;
}

static bool Deserialize(Entity texture) {
    if(!StreamOpen(texture, StreamMode_Read)) return false;

    StreamSeek(texture, StreamSeek_End);
    auto size = StreamTell(texture);
    StreamSeek(texture, 0);

    auto buffer = (stbi_uc*)malloc(size);
    StreamRead(texture, size, buffer);
    StreamClose(texture);

    int width, height, channels;
    auto result = stbi_info_from_memory(buffer, size, &width, &height, &channels) != 0;
    free(buffer);

    if(result) {
        SetTextureSize2D(texture, {width, height});

        switch(channels) {
            case 1:
                SetTextureFormat(texture, TextureFormat_R8);
                break;
            case 2:
                SetTextureFormat(texture, TextureFormat_RG8);
                break;
            case 3:
                SetTextureFormat(texture, TextureFormat_RGB8);
                break;
            case 4:
                SetTextureFormat(texture, TextureFormat_RGBA8);
                break;
        }
    }

    return result;
}

static bool Compress(Entity entity, u64 offset, u64 size, const void *pixels) {
    return false;
}

static bool Decompress(Entity entity, u64 offset, u64 size, void *pixels) {
    if(!StreamOpen(entity, StreamMode_Read)) return false;

    StreamSeek(entity, StreamSeek_End);
    auto fileSize = StreamTell(entity);
    StreamSeek(entity, 0);

    auto buffer = (stbi_uc*)malloc(fileSize);
    StreamRead(entity, fileSize, buffer);
    StreamClose(entity);

    int width, height, channels;
    auto pixelData = stbi_load_from_memory(buffer, fileSize, &width, &height, &channels, 0);
    if(pixelData) {
        Assert(entity, size <= (width * height * channels));
        memcpy(pixels, &pixelData[offset], size);
        stbi_image_free(pixelData);
    }

    free(buffer);

    return pixelData != NULL;
}

BeginUnit(StbImagePersistance)
    RegisterSerializer(Png, "image/png")
    RegisterStreamCompressor(Texture2D, "image/png")
EndUnit()
