//
// Created by Kim on 16-11-2018.
//

#ifndef PLAZA_SPRITE_H
#define PLAZA_SPRITE_H

#include <Core/NativeUtils.h>

struct Sprite {
    Entity SpriteTexture;
    rgba8 SpriteTint;
};

Unit(Sprite)
    Component(Sprite)
        ReferenceProperty(SubTexture2D, SpriteTexture)
        Property(rgba8, SpriteTint)

#endif //PLAZA_SPRITE_H
