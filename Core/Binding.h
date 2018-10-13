//
// Created by Kim on 30-09-2018.
//

#ifndef PLAZA_BINDING_H
#define PLAZA_BINDING_H

#include <Core/NativeUtils.h>

Unit(Binding)
    Component(Binding)
        Property(Entity, BindingProperty)
        Property(Entity, BindingTargetEntity)
        Property(Entity, BindingTargetProperty)
        ArrayProperty(ValueConverter, BindingConverters)

    Component(ValueConverterArgument)
        Property(Variant, ValueConverterArgumentValue)

    Component(ValueConverter)
        ReferenceProperty(Function, ValueConverterFunction)
        ArrayProperty(ValueConverterArgument, ValueConverterArguments)

    Component(Bindable)
        ArrayProperty(Binding, Bindings)

    Function(Bind, bool, Entity entity, Entity property, StringRef sourceBindingString)

#endif //PLAZA_BINDING_H
