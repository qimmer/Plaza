//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Entity.h>
#include <Foundation/Visibility.h>

static bool IsHidden(Entity entity) {
    auto owner = GetOwnership(entity).Owner;
    return GetVisibility(entity).Hidden || (IsEntityValid(owner) && IsHidden(owner));
}

static void OnVisibilityChanged(Entity changedEntity, const Visibility& oldData, const Visibility& newData) {
    Visibility data;
    for_entity_data(visibility, ComponentOf_Visibility(), &data) {
        auto hidden = IsHidden(visibility);
        if(hidden != data.HierarchiallyHidden) {
            data.HierarchiallyHidden = hidden;
            SetVisibility(visibility, data);
        }
    }
}

BeginUnit(Visibility)
    BeginComponent(Visibility)
        RegisterProperty(bool, Hidden)
        RegisterPropertyReadOnly(bool, HierarchiallyHidden)
    EndComponent()

    RegisterSystem(OnVisibilityChanged, ComponentOf_Visibility())
EndComponent()

