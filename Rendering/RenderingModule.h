//
// Created by Kim Johannsen on 08/01/2018.
//

#ifndef PLAZA_RENDERINGMODULE_H
#define PLAZA_RENDERINGMODULE_H

#include <Core/Module.h>

Module(Rendering)
    Unit(Rendering)
        Component(Rendering)
            ChildProperty(Uniform, RenderingUvOffsetScaleUniform)

#endif //PLAZA_RENDERINGMODULE_H
