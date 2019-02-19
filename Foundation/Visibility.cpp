//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Entity.h>
#include <Foundation/Visibility.h>

struct Visibility {
    bool Hidden, HierarchiallyHidden;
};

static bool IsHidden(Entity entity) {
    auto owner = GetOwner(entity);
    return GetHidden(entity) || (IsEntityValid(owner) && IsHidden(owner));
}

LocalFunction(OnHiddenChanged, void, Entity changedEntity) {
    for_entity(visibility, data, Visibility) {
        SetHierarchiallyHidden(visibility, IsHidden(visibility));
    }
}

BeginUnit(Visibility)
    BeginComponent(Visibility)
        RegisterProperty(bool, Hidden)
        RegisterPropertyReadOnly(bool, HierarchiallyHidden)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Hidden()), OnHiddenChanged, 0)
EndComponent()

