//
// Created by Kim Johannsen on 06-03-2018.
//

#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>
#include <memory.h>
#include <Rendering/Texture2D.h>
#include <Core/String.h>
#include <Rendering/Texture.h>
#include <Foundation/Persistance.h>
#include "StbImagePersistance.h"

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static bool SerializeImage(Entity entity) {
    return false;
}

static bool DeserializeImage(Entity entity) {
    Assert(StreamOpen(entity, StreamMode_Read));
    StreamSeek(entity, StreamSeek_End);
    auto size = StreamTell(entity);
    StreamSeek(entity, 0);

    auto buffer = (stbi_uc*)malloc(size);
    StreamRead(entity, size, buffer);
    StreamClose(entity);

    int width, height, channels;
    auto result = stbi_info_from_memory(buffer, size, &width, &height, &channels) != 0;
    free(buffer);

    if(result) {
        auto texture = CreateTexture2D(entity, "Texture");
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

        SetStreamPath(texture, GetStreamPath(entity));
    }

    return result;
}

static bool Decompress(Entity entity, u64 offset, u64 size, void *pixels) {
    Assert(StreamOpen(entity, StreamMode_Read));
    StreamSeek(entity, 0);

    auto buffer = (stbi_uc*)malloc(size);
    StreamRead(entity, size, buffer);
    StreamClose(entity);

    int width, height, channels;
    auto pixelData = stbi_load_from_memory(buffer, size, &width, &height, &channels, 0);
    if(pixelData) {
        Assert(size <= (width * height * channels));
        memcpy(pixels, &pixelData[offset], size);
        stbi_image_free(pixelData);
    }

    free(buffer);

    return pixelData != NULL;
}

static void OnServiceStart(Service service) {
    Serializer s {
        SerializeImage,
        DeserializeImage
    };
    StreamCompressor c {
        0,
        Decompress
    };
    AddFileType(".png", "image/png");
    AddSerializer("image/png", &s);
    AddStreamCompressor("image/png", &c);
}

static void OnServiceStop(Service service){
    RemoveFileType(".png");
    RemoveSerializer("image/png");
    RemoveStreamCompressor("image/png");
}

DefineService(StbImagePersistance)
    Subscribe(StbImagePersistanceStarted, OnServiceStart)
    Subscribe(StbImagePersistanceStopped, OnServiceStop)
EndService()
