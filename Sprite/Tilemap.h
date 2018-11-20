//
// Created by Kim on 16-11-2018.
//

#ifndef PLAZA_TILEMAP_H
#define PLAZA_TILEMAP_H

#include <Core/NativeUtils.h>

struct Tile {
    rgba8 TileColor;
    Entity TileTemplate;
};

struct TileSet {};

struct TileGrid {
    v2f TileGridSpacing;
    Entity TileGridSet, TileGridMap;
    float TileGridDepth;
};

struct TileFrame {
    rgba8 TileFrameColor;
    v2i TileFrameOffset;
};

struct TileAtlas {
    v2i TileAtlasDimensions;
};

Unit(Tilemap)
    // Represents a tile in a tileset
    Component(Tile)
        Property(rgba8, TileColor)
        ChildProperty(Sprite, TileTemplate)

    // A tileset represents a set of tiles (templates) that a tilemap can use
    Component(TileSet)
        ArrayProperty(Tile, TileSetTiles)

    // A tilegrid is a "spawner" that spawns tiles on a grid from a tileset according to the map texture's pixels matching the tiles' colors
    Component(TileGrid)
        ArrayProperty(Sprite, TileGridInstances)
        Property(v2f, TileGridSpacing)
        Property(float, TileGridDepth)
        ReferenceProperty(TileSet, TileGridSet)
        ReferenceProperty(Texture2D, TileGridMap)

    // A special SubTexture2D that translates an offset (by index instead of by pixel) into a tile atlas
    Component(TileFrame)
        Property(v2i, TileFrameOffset)

    // A special TextureAtlas that uses tile offsets instead of pixel offsets
    Component(TileAtlas)
        Property(v2i, TileAtlasDimensions)


#endif //PLAZA_TILEMAP_H
