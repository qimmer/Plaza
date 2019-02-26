//
// Created by Kim on 30-09-2018.
//

#ifndef PLAZA_BINDING_H
#define PLAZA_BINDING_H

#include <Core/NativeUtils.h>

struct Bindable {};

struct BindingListener {
    Entity BindingListenerEntity, BindingListenerProperty, BindingListenerIndirection;
};

struct Binding {
    Entity BindingTargetProperty, BindingSourceEntity;
};

struct BindingIndirection {
    Entity BindingIndirectionProperty;
};

struct ArrayBindingChild {
    Entity ArrayBindingChildSource;
};

Unit(Binding)
    Component(BindingListener)
        ReferenceProperty(Property, BindingListenerProperty)
        Property(Entity, BindingListenerEntity)
        ReferenceProperty(BindingIndirection, BindingListenerIndirection)

    Component(BindingIndirection)
        ReferenceProperty(Property, BindingIndirectionProperty)

    Component(Binding)
        ArrayProperty(BindingListener, BindingListeners) // Last source contains source with final value, others are sub-entities/path elements
        ReferenceProperty(Property, BindingTargetProperty)
        Property(Entity, BindingSourceEntity)
        ArrayProperty(BindingIndirection, BindingIndirections)

    Component(Bindable)
        ArrayProperty(Binding, Bindings)

    Component(ArrayBindingChild)
        Property(Entity, ArrayBindingChildSource)

    Function(GetBinding, Entity, Entity entity, Entity property)
#endif //PLAZA_BINDING_H
