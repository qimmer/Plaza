//
// Created by Kim on 16-11-2018.
//

#include "Tilemap.h"
#include "Sprite.h"
#include <Rendering/Texture2D.h>
#include <Foundation/Stream.h>
#include <Rendering/Texture.h>
#include <Core/Instance.h>
#include <Scene/Transform.h>
#include <Scene/Scene.h>

static void UpdateTileFrame(Entity atlas, Entity frame) {
    auto textureSize = GetTextureSize2D(atlas);
    auto tileAtlasDimensions = GetTileAtlasDimensions(atlas);
    v2i tileSize = {
            textureSize.x / tileAtlasDimensions.x,
            textureSize.y / tileAtlasDimensions.y
    };

    auto offset = GetTileFrameOffset(frame);
    SetSubTexture2DOffset(frame, {offset.x * tileSize.x, offset.y * tileSize.y});
    SetSubTexture2DSize(frame, tileSize);

}

static void UpdateTileAtlas(Entity atlas) {
    auto& frames = GetTextureAtlasSubTextures(atlas);

    for(auto i = 0; i < frames.size(); ++i) {
        auto frame = frames[i];
        UpdateTileFrame(atlas, frame);
    }
}

static void InstantiateTileGrid(Entity grid, Entity tileSet, v2i dimensions, u8 numChannels, const u8* buffer) {
    u32 numTiles = 0;
    auto& tiles = GetTileSetTiles(tileSet);
    auto spacing = GetTileGridSpacing(grid);
    auto depth = GetTileGridDepth(grid);
    auto offset = 0;
    for(auto y = 0; y < dimensions.y; ++y) {
        for(auto x = 0; x < dimensions.x; ++x) {
            rgba8 color;
            color.rgba = 0;
            color.a = 0xFF;

            memcpy(&color, buffer + offset, numChannels);

            Entity matchingTile = 0, matchingTileTemplate = 0;
            for(auto i = 0; i < numTiles; ++i) {
                if(GetTileColor(tiles[i]).rgba == color.rgba) {
                    matchingTile = tiles[i];
                    matchingTileTemplate = GetTileTemplate(matchingTile);
                    break;
                }
            }

            if(IsEntityValid(matchingTileTemplate)) {
                auto instance = AddTileGridInstances(grid);
                SetInstanceTemplate(instance, matchingTileTemplate);
                SetPosition3D(instance, {
                    spacing.x * x,
                    spacing.y * (dimensions.y - y),
                    depth
                });
            }

            offset += numChannels;
        }
    }
}

static void UpdateTileGrid(Entity grid) {
    auto map = GetTileGridMap(grid);
    auto tileSet = GetTileGridSet(grid);

    auto dimensions = GetTextureSize2D(map);
    auto numChannels = GetTextureFormatNumChannels(GetTextureFormat(map));

    auto size = dimensions.x * dimensions.y * numChannels;
    auto buffer = (u8*)malloc(size);

    if(!StreamOpen(map, StreamMode_Read)) return;

    if(StreamDecompress(map, 0, size, buffer)) {
        SetNumTileGridInstances(grid, 0);
        InstantiateTileGrid(grid, tileSet, dimensions, numChannels, buffer);
    }

    free(buffer);
    StreamClose(map);
}

LocalFunction(OnTileAtlasDimensionsChanged, void, Entity atlas, v2i oldValue, v2i newValue) {
    UpdateTileAtlas(atlas);
}

LocalFunction(OnTileFrameOffsetChanged, void, Entity frame, v2i oldValue, v2i newValue) {
    UpdateTileFrame(GetOwner(frame), frame);
}

LocalFunction(OnTileFrameAdded, void, Entity component, Entity entity) {
    UpdateTileFrame(GetOwner(entity), entity);
}

LocalFunction(OnTileGridChanged, void, Entity grid) {
    UpdateTileGrid(grid);
}

LocalFunction(OnStreamContentChanged, void, Entity stream) {
    if(HasComponent(stream, ComponentOf_Texture2D())) {
        for_entity(grid, data, TileGrid) {
            if(data->TileGridMap == stream) {
                UpdateTileGrid(grid);
            }
        }
    }
}

BeginUnit(Tilemap)
    BeginComponent(Tile)
        RegisterProperty(rgba8, TileColor)
        RegisterChildProperty(Sprite, TileTemplate)
    EndComponent()

    BeginComponent(TileSet)
        RegisterArrayProperty(Tile, TileSetTiles)
    EndComponent()

    BeginComponent(TileGrid)
        RegisterBase(SceneNode)
        RegisterArrayProperty(Sprite, TileGridInstances)
        RegisterReferenceProperty(TileSet, TileGridSet)
        RegisterProperty(v2f, TileGridSpacing)
        RegisterProperty(float, TileGridDepth)
        RegisterReferenceProperty(Texture2D, TileGridMap)
    EndComponent()

    BeginComponent(TileFrame)
        RegisterBase(SubTexture2D)
        RegisterProperty(v2i, TileFrameOffset)
    EndComponent()

    BeginComponent(TileAtlas)
        RegisterBase(TextureAtlas)
        RegisterProperty(v2i, TileAtlasDimensions)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TileAtlasDimensions()), OnTileAtlasDimensionsChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TileFrameOffset()), OnTileFrameOffsetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TileGridSet()), OnTileGridChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TileGridSpacing()), OnTileGridChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TileGridMap()), OnTileGridChanged, 0)
    RegisterSubscription(EventOf_StreamContentChanged(), OnStreamContentChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnTileFrameAdded, ComponentOf_TileFrame())
EndUnit()