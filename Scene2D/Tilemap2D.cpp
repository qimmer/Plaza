//
// Created by Kim Johannsen on 07-03-2018.
//

#include <Rendering/Texture2D.h>
#include <Rendering/SubTexture2D.h>
#include <Core/String.h>
#include <Rendering/Material.h>
#include <Rendering/UniformState.h>
#include <Scene/MeshInstance.h>
#include "Tilemap2D.h"
#include "Sprite.h"
#include "Transform2D.h"

struct Tilemap2D {
    v2i TileSize2D, TileSpacing2D;
};

DefineComponent(Tilemap2D)
    Dependency(Texture2D)
    DefineProperty(v2i, TileSize2D)
    DefineProperty(v2i, TileSpacing2D)
EndComponent()

DefineService(Tilemap2D)
EndService()

DefineComponentPropertyReactive(Tilemap2D, v2i, TileSize2D)
DefineComponentPropertyReactive(Tilemap2D, v2i, TileSpacing2D)

static void RegenerateTiles(Entity tilemap) {
    if(!HasTilemap2D(tilemap)) return;

    auto tilemapSize = GetTextureSize2D(tilemap);
    auto tilemapTileSize = GetTileSize2D(tilemap);
    auto tilemapTileSpacing = GetTileSpacing2D(tilemap);
    v2i tilemapInterval = {tilemapTileSize.x + tilemapTileSpacing.x, tilemapTileSize.y + tilemapTileSpacing.y};

    if(tilemapInterval.x <= 0 || tilemapInterval.y <= 0) return;

    v2i tileCount = {tilemapSize.x / tilemapInterval.x, tilemapSize.y / tilemapInterval.y};

    // Destroy eventual redundant tiles
    for(auto child = GetNextChildThat(tilemap, 0, HasSubTexture2D);
        IsEntityValid(child);
        child = GetNextChildThat(tilemap, child, HasSubTexture2D)) {
        int x, y;
        if(sscanf(GetName(child), "Tile_%d_%d", &x, &y) == 2) {
            if(x >= tileCount.x || y >= tileCount.y) {
                DestroyEntity(child);
            }
        }
    }

    String tilemapPath = GetEntityPath(tilemap);
    char tilePath[PATH_MAX];
    char textureUniformStatePath[PATH_MAX];
    char meshInstanceTemplatePath[PATH_MAX];

    // Create or update tiles
    for(auto x = 0; x < tileCount.x; ++x) {
        for(auto y = 0; y < tileCount.y; ++y) {
            sprintf(tilePath, "%s/Tile_%d_%d", tilemapPath.c_str(), x, y);
            sprintf(textureUniformStatePath, "%s/TextureUniformState", tilePath);
            sprintf(meshInstanceTemplatePath, "%s/MeshInstanceTemplate", tilePath);

            auto subTexture = CreateSubTexture2D(tilePath);
            SetSubTexture2DOffset(subTexture, {x * tilemapInterval.x, y * tilemapInterval.y});
            SetSubTexture2DSize(subTexture, {tilemapTileSize.x, tilemapTileSize.y});

            auto material = CreateMaterial(tilePath);
            SetMaterialProgram(material, GetSpriteProgram());
            SetMaterialRenderState(material, RenderState_STATE_RGB_WRITE | RenderState_STATE_ALPHA_WRITE | RenderState_STATE_CULL_CW);

            auto textureUniformState = CreateUniformState(textureUniformStatePath);
            SetUniformStateTexture(textureUniformState, subTexture);
            SetUniformStateUniform(textureUniformState, GetSpriteTextureUniform());

            auto meshInstanceTemplate = CreateMeshInstance(meshInstanceTemplatePath);
            SetMeshInstanceMaterial(meshInstanceTemplate, material);
            SetMeshInstanceMesh(meshInstanceTemplate, GetSpriteMesh());
            SetPosition2D(meshInstanceTemplate, {x, -y});
        }
    }
}

static bool ServiceStart() {
    SubscribeTexture2DChanged(RegenerateTiles);
    SubscribeTilemap2DChanged(RegenerateTiles);
    return true;
}

static bool ServiceStop() {
    UnsubscribeTexture2DChanged(RegenerateTiles);
    UnsubscribeTilemap2DChanged(RegenerateTiles);
    return true;
}