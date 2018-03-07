//
// Created by Kim Johannsen on 07-03-2018.
//

#ifndef PLAZA_TILEMAP2D_H
#define PLAZA_TILEMAP2D_H

#include <Core/Entity.h>

DeclareComponent(Tilemap2D)
DeclareService(Tilemap2D)

DeclareComponentProperty(Tilemap2D, v2i, TileSize2D)
DeclareComponentProperty(Tilemap2D, v2i, TileSpacing2D)

#endif //PLAZA_TILEMAP2D_H
