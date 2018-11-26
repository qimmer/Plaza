//
// Created by Kim on 16-11-2018.
//

#include "Sprite.h"
#include <Rendering/Texture2D.h>
#include <Rendering/Renderable.h>
#include <Scene/Transform.h>

LocalFunction(OnSpriteAdded, void, Entity component, Entity entity) {
    rgba8 white;
    white.rgba = 0xffffffff;

    SetSpriteTint(entity, white);
}

BeginUnit(Sprite)
    BeginComponent(Sprite)
        RegisterBase(Renderable)
        RegisterBase(Transform)
        RegisterReferenceProperty(SubTexture2D, SpriteTexture)
        RegisterProperty(rgba8, SpriteTint)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnSpriteAdded, ComponentOf_Sprite())
EndUnit()