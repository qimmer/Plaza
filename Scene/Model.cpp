//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Model.h"
#include "Transform.h"
#include <Rendering/Renderable.h>

struct Model {
};

BeginUnit(Model)
    BeginComponent(Model)
        RegisterBase(Renderable)
    EndComponent()
EndUnit()

