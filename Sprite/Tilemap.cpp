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
    auto textureSize = GetTexture2D(atlas).TextureSize2D;
    auto tileAtlasDimensions = GetTileAtlas(atlas).TileAtlasDimensions;
    v2i tileSize = {
            textureSize.x / tileAtlasDimensions.x,
            textureSize.y / tileAtlasDimensions.y
    };

    auto offset = GetTileFrame(frame).TileFrameOffset;
    SetSubTexture2D(frame, {{offset.x * tileSize.x, offset.y * tileSize.y}, tileSize});

}

static void UpdateTileAtlas(Entity atlas) {
    auto atlasData = GetTextureAtlas(atlas);
    for(auto frame : atlasData.TextureAtlasSubTextures) {
        UpdateTileFrame(atlas, frame);
    }
}

static void InstantiateTileGrid(Entity grid, Entity tileSet, v2i dimensions, u8 numChannels, const u8* buffer) {
    auto tileSetData = GetTileSet(tileSet);
    auto tileGridData = GetTileGrid(grid);

    tileGridData.TileGridInstances.SetSize(0);

    auto spacing = tileGridData.TileGridSpacing;
    auto depth = tileGridData.TileGridDepth;
    auto offset = 0;
    for(auto y = 0; y < dimensions.y; ++y) {
        for(auto x = 0; x < dimensions.x; ++x) {
            rgba8 color;
            color.rgba = 0;
            color.a = 0xFF;

            memcpy(&color, buffer + offset, numChannels);

            Entity matchingTile = 0, matchingTileTemplate = 0;
            for(auto tile : tileSetData.TileSetTiles) {
                if(GetTile(tile).TileColor.rgba == color.rgba) {
                    matchingTile = tile;
                    matchingTileTemplate = GetTile(matchingTile).TileTemplate;
                    break;
                }
            }

            if(IsEntityValid(matchingTileTemplate)) {
                auto instance = CreateEntity();
                tileGridData.TileGridInstances.Add(instance);

                SetInstance(instance, {matchingTileTemplate});
                auto transformData = GetTransform(instance);
                transformData.Position3D = {
                        spacing.x * x,
                        spacing.y * (dimensions.y - y),
                        depth
                };
                SetTransform(instance, transformData);
            }

            offset += numChannels;
        }
    }

    SetTileGrid(grid, tileGridData);
}

static void UpdateTileGrid(Entity grid) {
    auto tileGridData = GetTileGrid(grid);

    auto dimensions = GetTexture2D(tileGridData.TileGridMap).TextureSize2D;
    auto numChannels = GetTextureFormatNumChannels(GetTexture(tileGridData.TileGridMap).TextureFormat);

    auto size = dimensions.x * dimensions.y * numChannels;
    auto buffer = (u8*)malloc(size);

    if(!StreamOpen(tileGridData.TileGridMap, StreamMode_Read)) return;

    if(StreamDecompress(tileGridData.TileGridMap, 0, size, buffer)) {
        InstantiateTileGrid(grid, tileGridData.TileGridSet, dimensions, numChannels, buffer);
    }

    free(buffer);
    StreamClose(tileGridData.TileGridMap);
}

static void OnTileAtlasChanged(Entity atlas, const TileAtlas& oldData, const TileAtlas& newData) {
    UpdateTileAtlas(atlas);
}

static void OnTileFrameChanged(Entity frame, const TileFrame& oldData, const TileFrame& newData) {
    UpdateTileFrame(GetOwnership(frame).Owner, frame);
}

static void OnTileGridChanged(Entity grid, const TileGrid& oldData, const TileGrid& newData) {
    UpdateTileGrid(grid);
}

static void OnStreamChanged(Entity stream, const Stream& oldData, const Stream& newData) {
    if(HasComponent(stream, ComponentOf_Texture2D())) {
        TileGrid tileGridData;
        for_entity_data(grid, ComponentOf_TileGrid(), &tileGridData) {
            if(tileGridData.TileGridMap == stream) {
                UpdateTileGrid(grid);
            }
        }
    }
}

BeginUnit(Tilemap)
    BeginComponent(Tile)
        RegisterProperty(rgba8, TileColor)
        BeginChildProperty(TileTemplate)
    EndChildProperty()
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

    RegisterSystem(OnTileAtlasChanged, ComponentOf_TileAtlas())
    RegisterSystem(OnTileFrameChanged, ComponentOf_TileFrame())
    RegisterSystem(OnTileGridChanged, ComponentOf_TileGrid())
    RegisterSystem(OnStreamChanged, ComponentOf_Stream())
EndUnit()