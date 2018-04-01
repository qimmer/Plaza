//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Entity.h>
#include <Foundation/Visibility.h>


struct Visibility {
    Visibility() : Hidden(false) {}

    bool Hidden;
};

DefineComponent(Visibility)
    DefinePropertyReactive(bool, Hidden)
EndComponent()

DefineComponentPropertyReactive(Visibility, bool, Hidden)

