//
// Created by Kim on 30-09-2018.
//

#include "Binding.h"
#include "Debug.h"
#include "Identification.h"

struct Binding {
    Entity BindingProperty, BindingTargetEntity, BindingTargetProperty;
    Vector(BindingConverters, Entity, 8);
};

struct Bindable {
    Vector(Bindings, Entity, 16);
};

struct ValueConverterArgument {
    Variant ValueConverterArgumentValue;
};

struct ValueConverter {
    Entity ValueConverterFunction;
    Vector(ValueConverterArguments, Entity, 8);
};

static void EvaluateBinding(Entity binding) {
    auto entity = GetOwner(binding);
    auto data = GetBindingData(binding);

    Variant value;
    value.type = GetPropertyType(data->BindingProperty);
    GetPropertyValue(data->BindingProperty, entity, &value.data);

    for(auto i = 0; i < data->BindingConverters.Count; ++i) {
        auto converterData = GetValueConverterData(GetVector(data->BindingConverters)[i]);

        auto argumentTypes = (Type*)alloca(1 + (converterData->ValueConverterArguments.Count) * sizeof(Type));
        auto argumentData = (const void**)alloca(1 + (converterData->ValueConverterArguments.Count) * sizeof(const void*));

        argumentTypes[0] = TypeOf_Variant;
        argumentData[0] = &value;

        for(auto j = 1; j < converterData->ValueConverterArguments.Count; ++j) {
            auto argument = GetVector(converterData->ValueConverterArguments)[j-1];
            auto value = GetValueConverterArgumentData(argument);
            argumentTypes[j] = TypeOf_Variant;
            argumentData[j] = &value->ValueConverterArgumentValue;
        }

        CallFunction(converterData->ValueConverterFunction, &value, 1 + converterData->ValueConverterArguments.Count, argumentTypes, argumentData);
    }

    value = Cast(value, GetPropertyType(data->BindingTargetProperty));
    SetPropertyValue(data->BindingTargetProperty, data->BindingTargetEntity, &value.data);
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
    for_entity(existingBinding, data, Binding) {
        if(data->BindingTargetEntity == entity && data->BindingTargetProperty == property) {
            auto owner = GetOwner(existingBinding);
            auto index = GetArrayPropertyIndex(PropertyOf_Bindings(), owner, existingBinding);
            RemoveBindings(owner, index);
        }
    }

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

LocalFunction(OnPropertyChanged, void, Entity property, Entity entity, Variant oldValue, Variant newValue) {
    auto bindings = GetBindings(entity);
    if(!bindings) return;

    auto numBindings = GetNumBindings(entity);
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

    RegisterSubscription(PropertyChanged, OnPropertyChanged, 0)
EndUnit()