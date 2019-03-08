//
// Created by Kim on 30-09-2018.
//

#ifndef PLAZA_BINDING_H
#define PLAZA_BINDING_H

#include <Core/NativeUtils.h>

struct Indirection {
    Indirection() : IndirectionProperty(0), ListenerEntity(0) {}

    Entity IndirectionProperty;
    Entity ListenerEntity;
};

struct Binding {
    Binding() : BindingTargetProperty(0), BindingSourceEntity(0) {}

    Entity BindingTargetProperty, BindingSourceEntity;
	eastl::vector<Indirection> BindingIndirections;
};

struct Listener {
    Listener() : BindingEntity(0), BindingIndirectionIndex(0), BindingTargetProperty(0), ListenerProperty(0) {}

    Entity BindingEntity;
    u16 BindingIndirectionIndex;
    Entity BindingTargetProperty;
    Entity ListenerProperty;
};

struct EntityBindingData {
    eastl::vector<Listener> Listeners;
	eastl::vector<Binding> Bindings;
};

Unit(Binding)

void Bind(Entity entity, Entity property, Entity sourceEntity, const Entity* indirections, u32 numIndirections);
const Binding* GetBinding(Entity entity, Entity property);
void Unbind(Entity entity, Entity property);

EntityBindingData& GetBindingData(Entity entity);

#endif //PLAZA_BINDING_H
