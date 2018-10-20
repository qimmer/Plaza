//
// Created by Kim on 30-09-2018.
//

#include "Binding.h"
#include "Debug.h"
#include "Identification.h"

struct Binding {
    Entity BindingProperty, BindingTargetEntity, BindingTargetProperty;
};

struct Bindable {
};

struct ValueConverterArgument {
    Variant ValueConverterArgumentValue;
};

struct ValueConverter {
    Entity ValueConverterFunction;
};

static void EvaluateBinding(Entity binding) {
    auto entity = GetOwner(binding);
    auto data = GetBindingData(binding);

    auto value = GetPropertyValue(data->BindingProperty, entity);

    u32 numBindingConverters = 0;
    auto bindingConverters = GetBindingConverters(binding, &numBindingConverters);
    for(auto i = 0; i < numBindingConverters; ++i) {
        auto converterData = GetValueConverterData(bindingConverters[i]);

        u32 numArguments = 0;
        auto arguments = GetValueConverterArguments(bindingConverters[i], &numArguments);

        auto argumentData = (Variant*)alloca((1 + numArguments) * sizeof(Variant));

        argumentData[0] = value;

        for(auto j = 1; j < numArguments; ++j) {
            auto argument = arguments[j-1];
            auto value = GetValueConverterArgumentData(argument);
            argumentData[j] = value->ValueConverterArgumentValue;
        }

        value = CallFunction(converterData->ValueConverterFunction, 1 + numArguments, argumentData);
    }

    value = Cast(value, GetPropertyType(data->BindingTargetProperty));
    SetPropertyValue(data->BindingTargetProperty, data->BindingTargetEntity, value);
}

static void ParseValueConverter(Entity binding, char* valueConverterString) {
    auto name = valueConverterString;

    auto paramEnd = strrchr(valueConverterString, ')');

    while(*valueConverterString && *valueConverterString != '(') {
        valueConverterString++;
    }

    auto paramStart = valueConverterString;

    if(*paramStart) {
        *paramStart = '\0';
        paramStart++;
    }

    if(*paramEnd) {
        *paramEnd = '\0';
    }

    auto func = FindEntityByName(ComponentOf_Function(), name);
    if(!func) {
        Log(binding, LogSeverity_Error, "Value converter function '%s' not found.", name);
        return;
    }

    auto converter = AddBindingConverters(binding);
    SetValueConverterFunction(converter, func);

    if(paramStart && paramEnd && strlen(paramStart) > 0) {

    }

}

API_EXPORT bool Bind(Entity entity, Entity property, StringRef sourceBindingString) {
    auto len = strlen(sourceBindingString);
    auto buffer = (char*)alloca(len + 1);
    strcpy(buffer, sourceBindingString);

    auto atLocation = strrchr(buffer, '@');
    if(!atLocation) {
        Log(entity, LogSeverity_Error, "Binding string should be of the format 'valueConverter1(arg1, arg2):valueConverter2(arg1, arg2):source_property_name@source_entity_uuid': %s", sourceBindingString);
        return false;
    }
    *atLocation = '\0';

    auto sourceEntityUuid = atLocation + 1;

    char* sourcePropertyName = 0;
    char* converterString = 0;

    auto colonLocation = strrchr(buffer, ':');
    if(colonLocation) {
        *colonLocation = '\0';
        sourcePropertyName = colonLocation + 1;
        converterString = &buffer[0];
    } else {
        sourcePropertyName = &buffer[0];
    }

    // First, remove all existing bindings to this property
    for_entity(existingBinding, data, Binding, {
        if(data->BindingTargetEntity == entity && data->BindingTargetProperty == property) {
            auto owner = GetOwner(existingBinding);
            auto index = GetArrayPropertyIndex(PropertyOf_Bindings(), owner, existingBinding);
            RemoveBindings(owner, index);
        }
    });

    auto sourceEntity = FindEntityByUuid(sourceEntityUuid);
    if(!sourceEntity) {
        Log(entity, LogSeverity_Error, "Binding failed. Cannot find source entity: %s", sourceEntityUuid);
        return false;
    }

    auto sourceProperty = FindEntityByName(ComponentOf_Property(), sourcePropertyName);
    if(!sourceProperty) {
        Log(entity, LogSeverity_Error, "Binding failed. Cannot find source property: %s", sourcePropertyName);
        return false;
    }

    AddComponent(sourceEntity, ComponentOf_Bindable());
    auto binding = AddBindings(sourceEntity);
    SetBindingProperty(binding, sourceProperty);
    SetBindingTargetEntity(binding, entity);
    SetBindingTargetProperty(binding, property);

    while(converterString) {
        auto current = converterString;
        converterString = strchr(converterString, ':');
        if(converterString) {
            converterString[0] = '\0';
            converterString++;
        }

        ParseValueConverter(binding, current);
    }


    EvaluateBinding(binding);

    return true;
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    u32 numBindings = 0;
    auto bindings = GetBindings(entity, &numBindings);
    if(!bindings) return;

    for(auto i = 0; i < numBindings; ++i) {
        if(GetBindingProperty(bindings[i]) != property) continue;

        EvaluateBinding(bindings[i]);
    }
}

BeginUnit(Binding)
    BeginComponent(Binding)
        RegisterReferenceProperty(Property, BindingProperty)
        RegisterProperty(Entity, BindingTargetEntity)
        RegisterReferenceProperty(Property, BindingTargetProperty)
        RegisterArrayProperty(ValueConverter, BindingConverters)
    EndComponent()

    BeginComponent(Bindable)
        RegisterArrayProperty(Binding, Bindings)
    EndComponent()

    BeginComponent(ValueConverterArgument)
        RegisterProperty(Variant, ValueConverterArgumentValue)
    EndComponent()

    BeginComponent(ValueConverter)
        RegisterReferenceProperty(Function, ValueConverterFunction)
        RegisterArrayProperty(ValueConverterArgument, ValueConverterArguments)
    EndComponent()

    RegisterGenericPropertyChangedListener(OnPropertyChanged);
EndUnit()