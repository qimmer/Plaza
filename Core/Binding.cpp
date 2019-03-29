//
// Created by Kim on 30-09-2018.
//

#include <algorithm>
#include "Binding.h"
#include "Debug.h"
#include "Identification.h"
#include "Instance.h"

#include <EASTL/map.h>

static eastl::vector<EntityBindingData> BindingData;

struct ChangingBinding {
    Entity Entity, Property;
};

static Vector<ChangingBinding, 32> currentChangingBindings;

API_EXPORT EntityBindingData& GetBindingData(Entity entity) {
	auto entityIndex = GetEntityIndex(entity);

	if (BindingData.size() <= entityIndex) {
		BindingData.resize(entityIndex + 1);
	}
    
    return BindingData[entityIndex];
}

static inline void UpdateValue(const Listener& listener) {
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());

	auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, listener.BindingTargetProperty);
    auto& binding = GetBindingData(listener.BindingEntity).Bindings[propertyIndex];

    auto& indirection = binding.BindingIndirections[listener.BindingIndirectionIndex];

    if(!IsEntityValid(indirection.ListenerEntity)) return;

    auto propertyData = GetProperty(binding.BindingTargetProperty);

    Variant value = Variant_Default;

    if(GetProperty(indirection.IndirectionProperty).PropertyType == TypeOf_ChildArray) {
        auto nameToFind = indirection.IndirectionArrayName;
        auto elements = GetArrayPropertyElements(indirection.IndirectionProperty, indirection.ListenerEntity);
        auto numElements = GetArrayPropertyCount(indirection.IndirectionProperty, indirection.ListenerEntity);
        for(auto j = 0; j < numElements; ++j) {
            auto name = GetArrayChild(elements[j]).Name;
            if(name == nameToFind) {
                value = MakeVariant(Entity, elements[j]);
                break;
            }
        }
        Error(indirection.ListenerEntity, "Error resolving binding: Entity %s has no array element named %s.", GetIdentification(indirection.ListenerEntity).Uuid, nameToFind);
    } else {
        value = GetPropertyValue(indirection.IndirectionProperty, indirection.ListenerEntity);
    }


    // Do we need a cast?
    auto targetType = GetProperty(binding.BindingTargetProperty).PropertyType;
    if(propertyData.PropertyType != TypeOf_ChildArray && targetType != value.type) {
        value = Cast(value, targetType);
    }

    currentChangingBindings.push_back({listener.BindingEntity, binding.BindingTargetProperty}); // Allow change without breaking the binding
    SetPropertyValue(binding.BindingTargetProperty, listener.BindingEntity, value);
    currentChangingBindings.pop_back();
}

static void RemoveListeners(Entity self, const Binding& binding) {
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());

    auto& indirections = binding.BindingIndirections;

    auto numIndirections = indirections.size();
    for(u32 i = 0; i < numIndirections; ++i) {
        auto& indirection = indirections[i];

		auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, indirection.IndirectionProperty);
        auto& listeners = GetBindingData(indirection.ListenerEntity).Listeners[propertyIndex];
		auto size = listeners.size();
		for (u32 j = 0; j < size; ++j) {
			if (listeners[j].BindingEntity == self) {
				listeners[j] = listeners[size - 1];
				listeners.pop_back();
				break;
			}
		}
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
            if(GetProperty(indirection.IndirectionProperty).PropertyType == TypeOf_ChildArray) {
                auto nameToFind = indirection.IndirectionArrayName;
                auto elements = GetArrayPropertyElements(indirection.IndirectionProperty, sourceEntity);
                auto numElements = GetArrayPropertyCount(indirection.IndirectionProperty, sourceEntity);
                bool found = false;
                for(auto j = 0; j < numElements; ++j) {
                    auto name = GetArrayChild(elements[j]).Name;
                    if(name == nameToFind) {
                        sourceEntity = elements[j];
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    Error(self, "Error binding: Entity %s has no array element named %s.", GetIdentification(sourceEntity).Uuid, nameToFind);
                    sourceEntity = 0;
                }
            } else {
                sourceEntity = GetPropertyValue(indirection.IndirectionProperty, sourceEntity).as_Entity;
            }
        }
    }
}

static void AddListeners(Entity self, Binding& binding) {
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());

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

		auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, indirection.IndirectionProperty);
		GetBindingData(sourceEntity).Listeners[propertyIndex].push_back(listener);

        indirection.ListenerEntity = sourceEntity;

        if(sourceEntity) {
            if(i < (numIndirections - 1)) {
                if(GetProperty(indirection.IndirectionProperty).PropertyType == TypeOf_ChildArray) {
                    auto nameToFind = indirection.IndirectionArrayName;
                    auto elements = GetArrayPropertyElements(indirection.IndirectionProperty, sourceEntity);
                    auto numElements = GetArrayPropertyCount(indirection.IndirectionProperty, sourceEntity);
                    bool found = false;
                    for(auto j = 0; j < numElements; ++j) {
                        auto name = GetArrayChild(elements[j]).Name;
                        if(name == nameToFind) {
                            sourceEntity = elements[j];
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        Error(self, "Error binding: Entity %s has no array element named %s.", GetIdentification(sourceEntity).Uuid, nameToFind);
                        sourceEntity = 0;
                    }
                } else {
                    sourceEntity = GetPropertyValue(indirection.IndirectionProperty, sourceEntity).as_Entity;
                }
            } else {
                UpdateValue(listener);
            }
        }
    }
}

static inline void HandleListener(Entity self, const Listener& listener) {
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());

    if(listener.BindingIndirectionIndex > 0) {
		auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, listener.BindingTargetProperty);

        auto& binding = GetBindingData(listener.BindingEntity).Bindings[propertyIndex];

        // If an indirection changed, re-evaluate listeners
        RemoveListeners(listener.BindingEntity, binding);
        EvaluateIndirections(listener.BindingEntity, binding);
        AddListeners(listener.BindingEntity, binding);
    } else {
        // No indirection, so this is a value update, re-evaluate value
        UpdateValue(listener);
    }
}

void OnChanged(Entity entity, Entity component, const void *oldData, const void *newData) {
    static u32 propertyInfoIndex = GetComponentIndexByIndex(0, ComponentOf_Property());

    auto* bindingData = &GetBindingData(entity);

    for(auto& listener : bindingData->Listeners) {
        auto propertyIndex = listener.first;
        auto property = GetComponentEntity(propertyInfoIndex, propertyIndex);
        auto propertyComponent = GetOwnership(property).Owner;

        if (propertyComponent != component) continue;

        auto listenerVector = listener.second;
        for (auto listener : listenerVector) {
            HandleListener(entity, listener);
        }
        bindingData = &GetBindingData(entity);
    }

    for(auto& binding : bindingData->Bindings) {
        auto propertyIndex = binding.first;
        auto property = GetComponentEntity(propertyInfoIndex, propertyIndex);
        auto propertyComponent = GetOwnership(property).Owner;

        if(propertyComponent != component) continue;

        if(currentChangingBindings.back().Entity != entity && currentChangingBindings.back().Property != property) {
            auto targetProperty = binding.second.BindingTargetProperty;
            if(GetProperty(targetProperty).PropertyType != TypeOf_ChildArray) {
                RemoveListeners(entity, binding.second);

                bindingData->Bindings.erase(propertyIndex);
                return;
            }
        }
    }
}

API_EXPORT void Bind(Entity entity, Entity property, Entity sourceEntity, const Entity* indirections, const StringRef* indirectionArrayNames, u32 numIndirections) {

    auto& bindings = GetBindingData(entity).Bindings;
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());
	auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, property);

    auto existingBinding = bindings.find(propertyIndex);

    if(existingBinding != bindings.end()) {
        auto& binding = existingBinding->second;
        RemoveListeners(entity, binding);
        binding.BindingSourceEntity = sourceEntity;

        binding.BindingIndirections.resize(numIndirections);
        for(u32 i = 0; i < numIndirections; ++i) {
            binding.BindingIndirections[i].IndirectionProperty = indirections[i];
            binding.BindingIndirections[i].IndirectionArrayName = indirectionArrayNames ? indirectionArrayNames[i] : 0;
        }

        EvaluateIndirections(entity, binding);
        AddListeners(entity, binding);
    } else {
        auto& binding = bindings[propertyIndex];
        binding.BindingTargetProperty = property;
        binding.BindingSourceEntity = sourceEntity;

        binding.BindingIndirections.resize(numIndirections);
        for(u32 i = 0; i < numIndirections; ++i) {
            binding.BindingIndirections[i].IndirectionProperty = indirections[i];
            binding.BindingIndirections[i].IndirectionArrayName = indirectionArrayNames ? indirectionArrayNames[i] : 0;
        }

        EvaluateIndirections(entity, binding);
        AddListeners(entity, binding);
    }
}

API_EXPORT const Binding* GetBinding(Entity entity, Entity property) {
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());
	auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, property);

    auto& bindings = GetBindingData(entity).Bindings;
    auto existingBinding = bindings.find(propertyIndex);

    if(existingBinding != bindings.end()) {
        return &existingBinding->second;
    }

    return 0;
}

API_EXPORT void Unbind(Entity entity, Entity property) {
	static auto propertyInfoIndex = GetComponentIndex(ComponentOf_Component(), ComponentOf_Property());
	auto propertyIndex = GetComponentIndexByIndex(propertyInfoIndex, property);

    auto& bindings = GetBindingData(entity).Bindings;
    auto existingBinding = bindings.find(propertyIndex);

    if(existingBinding != bindings.end()) {
        RemoveListeners(entity, existingBinding->second);
        bindings.erase(existingBinding);
    }
}

BeginUnit(Binding)
    RegisterDeferredSystem(OnChanged, 0, 0)
EndUnit()