//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MODEL_H
#define PLAZA_MODEL_H

#include <Core/NativeUtils.h>

Unit(Model)
    Component(Model)
        ReferenceProperty(SubMesh, ModelSubMesh)
        ReferenceProperty(Material, ModelMaterial)

#endif //PLAZA_MESHINSTANCE_H
