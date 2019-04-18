//
// Created by Kim on 16-11-2018.
//

#include "Sprite.h"
#include <Rendering/Texture2D.h>
#include <Rendering/Renderable.h>
#include <Scene/Transform.h>

BeginUnit(Sprite)
    BeginComponent(Sprite)
        RegisterBase(Renderable)
        RegisterBase(Transform)
        RegisterReferenceProperty(SubTexture2D, SpriteTexture)
        RegisterProperty(rgba8, SpriteTint)
    EndComponent()
EndUnit()