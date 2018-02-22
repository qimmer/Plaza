//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_SPRITE_H
#define PLAZA_SPRITE_H

#include <Core/Entity.h>


    DeclareComponent(Sprite)
    DeclareService(Sprite)

    Entity GetSpriteTextureUniform();

    DeclareComponentProperty(Sprite, v2f, SpriteUvOffset)
    DeclareComponentProperty(Sprite, v2f, SpriteUvSize)
    DeclareComponentProperty(Sprite, v2f, SpriteUvScale)

#endif //PLAZA_SPRITE_H
