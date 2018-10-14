//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MATERIAL_H
#define PLAZA_MATERIAL_H


#include <Core/Entity.h>

struct Material {
    Entity MaterialProgram;
};

Enum(RenderState)

Unit(Material)
    Component(Material)
        ReferenceProperty(Program, MaterialProgram)

#endif //PLAZA_MATERIAL_H
