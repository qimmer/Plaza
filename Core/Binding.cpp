//
// Created by Kim on 30-09-2018.
//

#include "Binding.h"
#include "Debug.h"
#include "Identification.h"
#include "Instance.h"

struct BindingPropertyListener {
    Entity Listener, Property;
};

static Vector<BindingPropertyListener, 8>& GetBindingListeners(Entity sourceEntity) {
    static eastl::vector<Vector<BindingPropertyListener, 8>> BindingPropertyListeners;

    auto index = GetEntityIndex(sourceEntity);

    if(BindingPropertyListeners.size() <= index) {
        BindingPropertyListeners.resize(index + 1);
    }

    return BindingPropertyListeners[index];
}

static inline void AddBindingListener(Entity sourceEntity, Entity sourceProperty, Entity listener) {
    if(!IsEntityValid(sourceEntity) || !IsEntityValid(sourceProperty)) return;

    auto& listeners = GetBindingListeners(sourceEntity);

    BindingPropertyListener bindingPropertyListener;
    bindingPropertyListener.Listener = listener;
    bindingPropertyListener.Property = sourceProperty;

    listeners.push_back(bindingPropertyListener);
}

static inline void RemoveBindingListener(Entity sourceEntity, Entity sourceProperty, Entity listener) {
    if(!IsEntityValid(sourceEntity) || !IsEntityValid(sourceProperty)) return;

    auto entityIndex = GetEntityIndex(sourceEntity);
    auto& listeners = GetBindingListeners(sourceEntity);

    eastl::remove_if(listeners.begin(), listeners.end(), [&](const BindingPropertyListener& target) {
        return target.Listener == listener && target.Property == sourceProperty;
    });
}

static void UpdateBindingListeners(Entity binding) {
    auto data = GetBindingData(binding);
    if(!data) return;

    auto sourceEntity = data->BindingSourceEntity;
    if(!IsEntityValid(sourceEntity)) return;

    u32 numIndirections = 0;
    auto indirections = GetBindingIndirections(binding, &numIndirections);

    SetNumBindingListeners(binding, numIndirections);

    if(!numIndirections) return;

    u32 numListeners = 0;
    auto listeners = GetBindingListeners(binding, &numListeners);

    Variant value = Variant_Default;
    for(auto i = 0; i < numIndirections; ++i) {
        auto reverseIndex = numIndirections - 1 - i;
        auto property = GetBindingIndirectionProperty(indirections[reverseIndex]);
        SetBindingListenerEntity(listeners[reverseIndex], sourceEntity);
        SetBindingListenerProperty(listeners[reverseIndex], property);

        if(i < (numIndirections - 1)) {
            // If there are more indirections pending, this is an indirection and not the final property
            SetBindingListenerIndirection(listeners[reverseIndex], indirections[i]);
        }

        if(IsEntityValid(sourceEntity) && IsEntityValid(property)) {
            value = GetPropertyValue(property, sourceEntity);
            sourceEntity = value.as_Entity;
        } else {
            value = Variant_Default;
            value.type = GetPropertyType(data->BindingTargetProperty);
        }
    }

    value = Cast(value, GetPropertyType(data->BindingTargetProperty));
    SetPropertyValue(data->BindingTargetProperty, GetOwner(binding), value);
}

static inline void HandleListener(Entity listener, Variant newValue) {
    auto data = GetBindingListenerData(listener);
    auto binding = GetOwner(listener);

    if(data->BindingListenerIndirection) {
        // If an indirection changed, re-evaluate listeners
        UpdateBindingListeners(binding);
    } else {
        // No indirection, so this is a value update, re-evaluate value
        auto targetProperty = GetBindingTargetProperty(binding);

        auto value = Cast(newValue, GetPropertyType(targetProperty));
        SetPropertyValue(targetProperty, GetOwner(binding), value);
    }
}

LocalFunction(OnBindingListenerEntityChanged, void, Entity listener, Entity oldEntity, Entity newEntity) {
    RemoveBindingListener(oldEntity, GetBindingListenerProperty(listener), listener);
    AddBindingListener(newEntity, GetBindingListenerProperty(listener), listener);
}

LocalFunction(OnBindingListenerPropertyChanged, void, Entity listener, Entity oldProperty, Entity newProperty) {
    RemoveBindingListener(GetBindingListenerEntity(listener), oldProperty, listener);
    AddBindingListener(GetBindingListenerEntity(listener), newProperty, listener);
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    auto& listeners = GetBindingListeners(entity);

    auto propertyName = GetUuid(property);

    for(auto i = 0; i < listeners.size(); ++i) {
        auto& listener = listeners[i];

        auto listenerPropertyName = GetUuid(listener.Property);

        if(property != listener.Property) continue;

        HandleListener(listener.Listener, newValue);
    }
}

static bool ParseConverter(Entity binding, StringRef converterString) {

}

static StringRef Tokenize(char* string, char delimeter) {
    auto delimeterLocation = strrchr(string, delimeter);
    if(!delimeterLocation) return string;

    *delimeterLocation = '\0';

    return delimeterLocation + 1;
}

static void Split(char* string, char delimeter, StringRef* left, StringRef* right) {
    auto delimeterLocation = strrchr(string, delimeter);
    if(delimeterLocation) {
        *delimeterLocation = '\0';
    }

    *left = string;
    *right = delimeterLocation ? (delimeterLocation + 1) : "";
}

static bool ParseBinding(Entity binding, StringRef sourceBindingString) {
    auto data = GetBindingData(binding);

    SetNumBindingIndirections(binding, 0);

    auto len = strlen(sourceBindingString);
    auto buffer = (char*)alloca(len + 1);
    strcpy(buffer, sourceBindingString);

    StringRef sourceEntityUuid, propertiesString;
    Split(buffer, '@', &propertiesString, &sourceEntityUuid);

    StringRef propertyName;
    do {
        propertyName = Tokenize((char*)propertiesString, '.');
        auto prototypeNameLen = strlen(propertyName);

        char *propertyUuid = (char*)alloca(prototypeNameLen + 1 + 9);
        sprintf(propertyUuid, "Property.%s", propertyName);
        auto property = FindEntityByUuid(propertyUuid);

        if(!IsEntityValid(property)) {
            Error(binding, "Cannot find property with Uuid '%s'.", propertyUuid);
            return false;
        }

        auto indirection = AddBindingIndirections(binding);
        SetBindingIndirectionProperty(indirection, property);
    }
    while(propertyName > propertiesString);

    static const StringRef selfUuid = Intern("self");

    if(*sourceEntityUuid && sourceEntityUuid != selfUuid) {
        auto reference = AddUnresolvedReferences(binding);
        SetUnresolvedReferenceUuid(reference, sourceEntityUuid);
        SetUnresolvedReferenceProperty(reference, PropertyOf_BindingSourceEntity());
    } else {
        // If no '@', we use self as our source entity
        SetBindingSourceEntity(binding, GetOwner(binding));
    }

    return true;
}

LocalFunction(OnBindingSourceEntityChanged, void, Entity binding) {
    UpdateBindingListeners(binding);
}

LocalFunction(OnBindingIndirectionPropertyChanged, void, Entity indirection) {
    UpdateBindingListeners(GetOwner(indirection));
}

API_EXPORT bool Bind(Entity entity, Entity property, StringRef sourceBindingString) {
    // First, remove eventual existing binding of this property
    for_children(existingBinding, Bindings, entity, {
        auto data = GetBindingData(existingBinding);
        if(data->BindingTargetProperty == property) {
            return ParseBinding(existingBinding, sourceBindingString);
        }
    });

    auto binding = AddBindings(entity);
    SetBindingTargetProperty(binding, property);

    return ParseBinding(binding, sourceBindingString);
}

API_EXPORT Entity GetBinding(Entity entity, Entity property) {
    for_children(binding, Bindings, entity, {
        auto data = GetBindingData(binding);
        if(data->BindingTargetProperty == property) {
            return binding;
        }
    });

    return 0;
}

BeginUnit(Binding)

    BeginComponent(BindingListener)
        RegisterReferenceProperty(Property, BindingListenerProperty)
        RegisterProperty(Entity, BindingListenerEntity)
        RegisterReferenceProperty(BindingIndirection, BindingListenerIndirection)
    EndComponent()

    BeginComponent(BindingIndirection)
        RegisterReferenceProperty(Property, BindingIndirectionProperty)
    EndComponent()

    BeginComponent(Binding)
        RegisterArrayPropertyReadOnly(BindingListener, BindingListeners)
        RegisterReferenceProperty(Property, BindingTargetProperty)
        RegisterProperty(Entity, BindingSourceEntity)
        RegisterArrayProperty(BindingIndirection, BindingIndirections)
    EndComponent()

    BeginComponent(Bindable)
        RegisterArrayProperty(Binding, Bindings)
    EndComponent()

    RegisterGenericPropertyChangedListener(OnPropertyChanged);

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingListenerEntity()), OnBindingListenerEntityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingListenerProperty()), OnBindingListenerPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingSourceEntity()), OnBindingSourceEntityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingIndirectionProperty()), OnBindingIndirectionPropertyChanged, 0)
EndUnit()