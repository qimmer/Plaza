//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Entity.h>
#include <Foundation/Visibility.h>

struct Visibility {
    bool Hidden;
};

BeginUnit(Visibility)
    BeginComponent(Visibility)
        RegisterProperty(bool, Hidden)
    EndComponent()
EndComponent()

