//
// Created by Kim on 30-09-2018.
//

#include <algorithm>
#include "Binding.h"
#include "Debug.h"
#include "Identification.h"
#include "Instance.h"

#include <EASTL/map.h>

static eastl::vector<PropertyBindingData> BindingData;

struct ChangingBinding {
    Entity Entity, Property;
};

static Vector<ChangingBinding, 32> currentChangingBindings;

API_EXPORT PropertyBindingData& GetBindingData(Entity property) {
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    if(BindingData.size() <= propertyIndex) {
        BindingData.resize(propertyIndex + 1);
    }
    return BindingData[propertyIndex];
}

static inline void UpdateValue(const Listener& listener) {
    auto& binding = GetBindingData(listener.BindingTargetProperty).Bindings[listener.BindingEntity];

    auto& indirection = binding.BindingIndirections[listener.BindingIndirectionIndex];

    if(!IsEntityValid(indirection.ListenerEntity)) return;

    auto propertyData = GetPropertyData(binding.BindingTargetProperty);

    auto value = GetPropertyValue(indirection.IndirectionProperty, indirection.ListenerEntity);

    // Do we need a cast?
    auto targetType = GetPropertyType(binding.BindingTargetProperty);
    if(propertyData->PropertyKind == PropertyKind_Value && targetType != value.type) {
        value = Cast(value, targetType);
    }

    currentChangingBindings.push_back({listener.BindingEntity, binding.BindingTargetProperty}); // Allow change without breaking the binding
    SetPropertyValue(binding.BindingTargetProperty, listener.BindingEntity, value);
    currentChangingBindings.pop_back();
}

static void RemoveListeners(Entity self, const Binding& binding) {
    auto& indirections = binding.BindingIndirections;

    auto numIndirections = indirections.size();
    for(u32 i = 0; i < numIndirections; ++i) {
        auto& indirection = indirections[i];

        auto& listeners = GetBindingData(indirection.IndirectionProperty).Listeners[indirection.ListenerEntity];
		std::remove_if(listeners.begin(), listeners.end(), [&](auto& value) { return value.BindingEntity == self; });
    }
}

static void EvaluateIndirections(Entity self, Binding& binding) {
    auto sourceEntity = binding.BindingSourceEntity ? binding.BindingSourceEntity : self;
    auto& indirections = binding.BindingIndirections;

    auto numIndirections = indirections.size();
    for(u32 i = 0; i < numIndirections; ++i) {
        auto reverseIndex = numIndirections - 1 - i;

        auto& indirection = indirections[reverseIndex];
        indirection.ListenerEntity = sourceEntity;

        if(sourceEntity && i < (numIndirections - 1)) {
            sourceEntity = GetPropertyValue(indirection.IndirectionProperty, sourceEntity).as_Entity;
        }
    }
}

static void AddListeners(Entity self, Binding& binding) {
    auto sourceEntity = binding.BindingSourceEntity ? binding.BindingSourceEntity : self;

    auto& indirections = binding.BindingIndirections;

    auto numIndirections = indirections.size();
    for(u32 i = 0; i < numIndirections; ++i) {
        auto reverseIndex = numIndirections - 1 - i;

        auto& indirection = indirections[reverseIndex];

		Listener listener;
        listener.BindingEntity = self;
        listener.BindingTargetProperty = binding.BindingTargetProperty;
        listener.BindingIndirectionIndex = (u16)reverseIndex;
        listener.ListenerProperty = indirection.IndirectionProperty;
		GetBindingData(indirection.IndirectionProperty).Listeners[sourceEntity].push_back(listener);

        indirection.ListenerEntity = sourceEntity;

        if(sourceEntity) {
            if(i < (numIndirections - 1)) {
                sourceEntity = GetPropertyValue(indirection.IndirectionProperty, sourceEntity).as_Entity;
            } else {
                UpdateValue(listener);
            }
        }
    }
}

static inline void HandleListener(Entity self, const Listener& listener, Variant oldValue, Variant newValue) {
    if(listener.BindingIndirectionIndex > 0) {
        auto& binding = GetBindingData(listener.BindingTargetProperty).Bindings[listener.BindingEntity];

        // If an indirection changed, re-evaluate listeners
        RemoveListeners(listener.BindingEntity, binding);
        EvaluateIndirections(listener.BindingEntity, binding);
        AddListeners(listener.BindingEntity, binding);
    } else {
        // No indirection, so this is a value update, re-evaluate value
        UpdateValue(listener);
    }
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    auto& bindingData = GetBindingData(property);
    auto& listeners = bindingData.Listeners;
    auto listenerIt = listeners.find(entity);
    if(listenerIt != listeners.end()) {
		for (auto& listener : listenerIt->second) {
			HandleListener(entity, listener, oldValue, newValue);
		}
    }

    // If this property is bound and the change was not triggered by a binding value update, break the binding!
    auto binding = bindingData.Bindings.find(entity);
    if(binding != bindingData.Bindings.end() && currentChangingBindings.back().Entity != entity && currentChangingBindings.back().Property != property) {
        auto targetProperty = binding->second.BindingTargetProperty;
        if(GetPropertyKind(targetProperty) == PropertyKind_Value) {
            RemoveListeners(entity, binding->second);

            bindingData.Bindings.erase(entity);
            return;
        }
    }

}

LocalFunction(OnComponentRemoved, void, Entity component, Entity entity) {
    for_children(property, Properties, component) {
        Unbind(entity, property);
    }
}

API_EXPORT void Bind(Entity entity, Entity property, Entity sourceEntity, const Entity* indirections, u32 numIndirections) {

    auto& bindings = GetBindingData(property).Bindings;
    auto existingBinding = bindings.find(entity);

    if(existingBinding != bindings.end()) {
        auto& binding = existingBinding->second;
        RemoveListeners(entity, binding);
        binding.BindingSourceEntity = sourceEntity;

        binding.BindingIndirections.resize(numIndirections);
        for(auto i = 0; i < numIndirections; ++i) {
            binding.BindingIndirections[i].IndirectionProperty = indirections[i];
        }

        EvaluateIndirections(entity, binding);
        AddListeners(entity, binding);
    } else {
        auto& binding = bindings[entity];
        binding.BindingTargetProperty = property;
        binding.BindingSourceEntity = sourceEntity;

        binding.BindingIndirections.resize(numIndirections);
        for(auto i = 0; i < numIndirections; ++i) {
            binding.BindingIndirections[i].IndirectionProperty = indirections[i];
        }

        EvaluateIndirections(entity, binding);
        AddListeners(entity, binding);
    }
}

API_EXPORT const Binding* GetBinding(Entity entity, Entity property) {
    auto& bindings = GetBindingData(property).Bindings;
    auto existingBinding = bindings.find(entity);

    if(existingBinding != bindings.end()) {
        return &existingBinding->second;
    }

    return 0;
}

API_EXPORT void Unbind(Entity entity, Entity property) {
    auto& bindings = GetBindingData(property).Bindings;
    auto existingBinding = bindings.find(entity);

    if(existingBinding != bindings.end()) {
        RemoveListeners(entity, existingBinding->second);
        bindings.erase(existingBinding);
    }
}

BeginUnit(Binding)
    RegisterGenericPropertyChangedListener(OnPropertyChanged);
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnComponentRemoved, 0)
EndUnit()