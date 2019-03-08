//
// Created by Kim on 30-09-2018.
//

#include <algorithm>
#include "Binding.h"
#include "Debug.h"
#include "Identification.h"
#include "Instance.h"

#include <EASTL/map.h>

static eastl::map<Entity, EntityBindingData> BindingData;

struct ChangingBinding {
    Entity Entity, Property;
};

static Vector<ChangingBinding, 32> currentChangingBindings;

API_EXPORT EntityBindingData& GetBindingData(Entity entity) {
    return BindingData[entity];
}

static inline void UpdateValue(const Listener& listener) {
    auto& bindings = GetBindingData(listener.BindingEntity).Bindings;
    auto binding = std::find_if(bindings.begin(), bindings.end(), [=](const Binding& binding) {
        return binding.BindingTargetProperty == listener.BindingTargetProperty;
    });

    auto& indirection = binding->BindingIndirections[listener.BindingIndirectionIndex];

    if(!IsEntityValid(indirection.ListenerEntity)) return;

    auto propertyData = GetPropertyData(binding->BindingTargetProperty);

    auto value = GetPropertyValue(indirection.IndirectionProperty, indirection.ListenerEntity);

    // Do we need a cast?
    auto targetType = GetPropertyType(binding->BindingTargetProperty);
    if(propertyData->PropertyKind == PropertyKind_Value && targetType != value.type) {
        value = Cast(value, targetType);
    }

    currentChangingBindings.push_back({listener.BindingEntity, binding->BindingTargetProperty}); // Allow change without breaking the binding
    SetPropertyValue(binding->BindingTargetProperty, listener.BindingEntity, value);
    currentChangingBindings.pop_back();
}

static void RemoveListeners(Entity self, const Binding& binding) {
    auto& indirections = binding.BindingIndirections;

    auto numIndirections = indirections.size();
    for(u32 i = 0; i < numIndirections; ++i) {
        auto& indirection = indirections[i];

        auto& listenerData = GetBindingData(indirection.ListenerEntity);

        // Find listener related to binding and indirection
        s32 listenerIndex = -1;
        for(auto j = 0; j < listenerData.Listeners.size(); j++) {
            auto& listener = listenerData.Listeners[j];
            if(listener.ListenerProperty == indirection.IndirectionProperty && listener.BindingEntity == self && listener.BindingIndirectionIndex == i) {
                listenerIndex = j;
            }
        }

        listenerData.Listeners.erase(listenerData.Listeners.begin() + listenerIndex);
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

        auto& sourceBindingData = GetBindingData(sourceEntity);

        Listener listener;
        listener.BindingEntity = self;
        listener.BindingTargetProperty = binding.BindingTargetProperty;
        listener.BindingIndirectionIndex = (u16)reverseIndex;
        listener.ListenerProperty = indirection.IndirectionProperty;

        sourceBindingData.Listeners.push_back(listener);

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
        auto& bindings = GetBindingData(listener.BindingEntity).Bindings;

        s32 bindingIndex = -1;
        for(auto i = 0; i < bindings.size(); ++i) {
            auto& binding = bindings[i];
            if(binding.BindingTargetProperty == listener.BindingTargetProperty) {
                bindingIndex = i;
                break;
            }
        }

        // If an indirection changed, re-evaluate listeners
        RemoveListeners(listener.BindingEntity, bindings[bindingIndex]);
        EvaluateIndirections(listener.BindingEntity, bindings[bindingIndex]);
        AddListeners(listener.BindingEntity, bindings[bindingIndex]);
    } else {
        // No indirection, so this is a value update, re-evaluate value
        UpdateValue(listener);
    }
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    auto& bindingData = GetBindingData(entity);
    if(bindingData.Listeners.size()) {
        auto listeners = bindingData.Listeners;

        for(auto i = 0; i < listeners.size(); ++i) {
            auto& listener = listeners[i];

            if(property != listener.ListenerProperty) continue;

            HandleListener(entity, listener, oldValue, newValue);
        }
    }

    auto& bindings = GetBindingData(entity).Bindings;
    auto binding = std::find_if(bindings.begin(), bindings.end(), [=](const Binding& binding) {
        return binding.BindingTargetProperty == property;
    });

    // If this property is bound and the change was not triggered by a binding value update, break the binding!
    if(binding != bindings.end() && currentChangingBindings.back().Entity != entity && currentChangingBindings.back().Property != property) {
        auto targetProperty = binding->BindingTargetProperty;
        if(GetPropertyKind(targetProperty) == PropertyKind_Value) {
            RemoveListeners(entity, *binding);

            bindings.erase(binding);

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
    auto& bindings = GetBindingData(entity).Bindings;
    auto existingBinding = std::find_if(bindings.begin(), bindings.end(), [&](const Binding& binding) {
        return binding.BindingTargetProperty == property;
    });

    if(existingBinding != bindings.end()) {
        RemoveListeners(entity, *existingBinding);
        existingBinding->BindingSourceEntity = sourceEntity;

        existingBinding->BindingIndirections.resize(numIndirections);
        for(auto i = 0; i < numIndirections; ++i) {
            existingBinding->BindingIndirections[i].IndirectionProperty = indirections[i];
        }

        EvaluateIndirections(entity, *existingBinding);
        AddListeners(entity, *existingBinding);
    } else {
        Binding binding;
        binding.BindingTargetProperty = property;
        binding.BindingSourceEntity = sourceEntity;

        binding.BindingIndirections.resize(numIndirections);
        for(auto i = 0; i < numIndirections; ++i) {
            binding.BindingIndirections[i].IndirectionProperty = indirections[i];
        }

        bindings.push_back(binding);
        EvaluateIndirections(entity, bindings.back());
        AddListeners(entity, bindings.back());
    }
}

API_EXPORT const Binding* GetBinding(Entity entity, Entity property) {
    auto& bindings = GetBindingData(entity).Bindings;
    auto binding = std::find_if(bindings.begin(), bindings.end(), [=](const Binding& binding) {
        return binding.BindingTargetProperty == property;
    });

    return binding == bindings.end() ? NULL : &*binding;
}

API_EXPORT void Unbind(Entity entity, Entity property) {
    auto& bindings = GetBindingData(entity).Bindings;
    auto binding = std::find_if(bindings.begin(), bindings.end(), [=](const Binding& binding) {
        return binding.BindingTargetProperty == property;
    });

    if(binding != bindings.end()) {
        RemoveListeners(entity, *binding);
        bindings.erase(binding);
    }
}

BeginUnit(Binding)
    RegisterGenericPropertyChangedListener(OnPropertyChanged);
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnComponentRemoved, 0)
EndUnit()