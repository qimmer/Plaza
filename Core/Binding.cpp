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

static eastl::vector<Vector<BindingPropertyListener, 8>> BindingPropertyListeners;

static Vector<Entity, 32> currentChangingBindings;

static Vector<BindingPropertyListener, 8>& GetBindingListeners(Entity sourceEntity) {
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

static inline void UpdateValue(Entity binding) {
    u32 numListeners = 0;
    auto listeners = GetBindingListeners(binding, &numListeners);
    auto data = GetBindingListenerData(listeners[0]);

    auto self = GetOwner(binding);
    if(!self) return;

    auto targetProperty = GetBindingTargetProperty(binding);

    if(!IsEntityValid(data->BindingListenerEntity)) return;

    auto propertyData = GetPropertyData(data->BindingListenerProperty);

    if(propertyData->PropertyKind == PropertyKind_Array) {
        u32 numChildren = 0;
        auto templateChildren = GetArrayPropertyElements(data->BindingListenerProperty, data->BindingListenerEntity, &numChildren);

        SetArrayPropertyCount(targetProperty, self, 0);
        SetArrayPropertyCount(targetProperty, self, numChildren);

        templateChildren = GetArrayPropertyElements(data->BindingListenerProperty, data->BindingListenerEntity, &numChildren);
        auto instanceChildren = GetArrayPropertyElements(targetProperty, self, &numChildren);

        for(auto i = 0; i < numChildren; ++i) {
            SetInstanceTemplate(instanceChildren[i], templateChildren[i]);
        }
    } else {
        auto value = GetPropertyValue(data->BindingListenerProperty, data->BindingListenerEntity);

        // Do we need a cast?
        auto targetType = GetPropertyType(targetProperty);
        if(propertyData->PropertyKind == PropertyKind_Value && targetType != value.type) {
            value = Cast(value, targetType);
        }

        currentChangingBindings.push_back(binding); // Allow change without breaking the binding
        SetPropertyValue(targetProperty, self, value);
        currentChangingBindings.pop_back();
    }
}

static void UpdateBindingListeners(Entity binding) {
    auto data = GetBindingData(binding);
    if(!IsEntityValid(data->BindingTargetProperty)) return;

    auto self = GetOwner(binding);

    auto sourceEntity = data->BindingSourceEntity;

    // If binding is not set, the source is "self"
    if(!IsEntityValid(sourceEntity)) sourceEntity = self;

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

            if(IsEntityValid(sourceEntity) && IsEntityValid(property)) {
                sourceEntity = GetPropertyValue(property, sourceEntity).as_Entity;
            }
        } else {
            SetBindingListenerIndirection(listeners[reverseIndex], 0);
        }
    }

    UpdateValue(binding);
}

static inline void HandleListener(Entity listener, Variant oldValue, Variant newValue) {
    auto data = GetBindingListenerData(listener);
    auto binding = GetOwner(listener);

    if(data && binding) {
        if(data->BindingListenerIndirection) {
            // If an indirection changed, re-evaluate listeners
            UpdateBindingListeners(binding);
        } else {
            // No indirection, so this is a value update, re-evaluate value
            auto targetProperty = GetBindingTargetProperty(binding);

            UpdateValue(binding);
        }
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

API_EXPORT Entity GetBinding(Entity entity, Entity property) {
    for_children(binding, Bindings, entity) {
        auto data = GetBindingData(binding);
        if(data && data->BindingTargetProperty == property) {
            return binding;
        }
    }

    return 0;
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    auto& listeners = GetBindingListeners(entity);

    for(auto i = 0; i < listeners.size(); ++i) {
        auto& listener = listeners[i];

        if(property != listener.Property) continue;

        HandleListener(listener.Listener, oldValue, newValue);
    }

    // If this property is bound and the change was not triggered by a binding value update, break the binding!
    auto binding = GetBinding(entity, property);
    if(binding && currentChangingBindings.back() != binding) {

        auto property = GetBindingTargetProperty(binding);
        if(GetPropertyKind(property) == PropertyKind_Value) {
            RemoveBindingsByValue(entity, binding);
        }
    }
}

LocalFunction(OnBindingSourceEntityChanged, void, Entity binding) {
    UpdateBindingListeners(binding);
}

LocalFunction(OnBindingIndirectionPropertyChanged, void, Entity indirection) {
    auto binding = GetOwner(indirection);
    if(binding) {
        UpdateBindingListeners(binding);
    }
}

LocalFunction(OnBindingTargetPropertyChanged, void, Entity binding, Entity oldProperty, Entity newProperty) {
    UpdateBindingListeners(binding);
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    if(HasComponent(entity, ComponentOf_BindingIndirection())) {
        UpdateBindingListeners(oldOwner);
    }

    if(HasComponent(entity, ComponentOf_Binding())) {
        UpdateBindingListeners(entity);
    }
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

    BeginComponent(ArrayBindingChild)
        RegisterProperty(Entity, ArrayBindingChildSource)
    EndComponent()

    RegisterGenericPropertyChangedListener(OnPropertyChanged);

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingListenerEntity()), OnBindingListenerEntityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingListenerProperty()), OnBindingListenerPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingSourceEntity()), OnBindingSourceEntityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingTargetProperty()), OnBindingTargetPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BindingIndirectionProperty()), OnBindingIndirectionPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
EndUnit()