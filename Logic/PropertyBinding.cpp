//
// Created by Kim Johannsen on 05-04-2018.
//

#include "PropertyBinding.h"

struct PropertyBinding {
    Entity PropertyBindingSourceEntity, PropertyBindingDestinationEntity;
    Property PropertyBindingSourceProperty, PropertyBindingDestinationProperty;
};

DefineComponent(PropertyBinding)
    DefineProperty(Entity, PropertyBindingSourceEntity)
    DefineProperty(Entity, PropertyBindingDestinationEntity)
    DefineProperty(Property, PropertyBindingSourceProperty)
    DefineProperty(Property, PropertyBindingDestinationProperty)
EndComponent()

DefineComponentPropertyReactive(PropertyBinding, Entity, PropertyBindingSourceEntity)
DefineComponentPropertyReactive(PropertyBinding, Entity, PropertyBindingDestinationEntity)
DefineComponentPropertyReactive(PropertyBinding, Property, PropertyBindingSourceProperty)
DefineComponentPropertyReactive(PropertyBinding, Property, PropertyBindingDestinationProperty)

typedef void(*GenericSubscriptionHandler)(Entity entity, void* func);

static Pool<Vector<Entity, 16>> EntitySourceBindings;
static Pool<Vector<Entity, 16>> EntityDestinationBindings;

static inline void OnDataChanged(Entity propertyBinding, Entity changedEntity, Property changedProperty, Entity boundEntity, Property boundProperty) {
    static char srcValueBuffer[sizeof(m4x4f)];
    static char dstValueBuffer[sizeof(m4x4f)];
    memset(srcValueBuffer, 0, sizeof(m4x4f));
    memset(dstValueBuffer, 0, sizeof(m4x4f));

    GetPropertyGenericGetter(changedProperty)(changedEntity, &srcValueBuffer);
    GetPropertyGenericGetter(boundProperty)(boundEntity, &dstValueBuffer);

    if(memcmp(srcValueBuffer, dstValueBuffer, sizeof(m4x4f)) != 0) {
        if(GetPropertyType(boundProperty) != GetPropertyType(changedProperty)) {
            Log(LogChannel_Core, LogSeverity_Error, "PropertyBinding property types mismatch on '%s'", GetEntityPath(propertyBinding));
            return;
        }
        GetPropertyGenericSetter(boundProperty)(boundEntity, srcValueBuffer);
    }
}

static void OnSourceEntityDataChanged(Entity entity, Type changedComponent) {
    auto entityIndex = GetHandleIndex(entity);
    auto& bindings = EntitySourceBindings[entityIndex];

    for(auto& binding : bindings) {
        auto data = GetPropertyBinding(binding);

        if(GetPropertyOwner(data->PropertyBindingSourceProperty) != changedComponent) continue;

        OnDataChanged(
            binding,
            data->PropertyBindingSourceEntity,
            data->PropertyBindingSourceProperty,
            data->PropertyBindingDestinationEntity,
            data->PropertyBindingDestinationProperty);
    }
}

static void OnDestinationEntityDataChanged(Entity entity, Type changedComponent) {
    auto entityIndex = GetHandleIndex(entity);
    auto& bindings = EntityDestinationBindings[entityIndex];

    for(auto& binding : bindings) {
        auto data = GetPropertyBinding(binding);

        if(GetPropertyOwner(data->PropertyBindingDestinationProperty) != changedComponent) continue;

        OnDataChanged(
            binding,
            data->PropertyBindingDestinationEntity,
            data->PropertyBindingDestinationProperty,
            data->PropertyBindingSourceEntity,
            data->PropertyBindingSourceProperty);
    }
}

static void OnPropertyBindingSourceEntityChanged(Entity entity, Entity oldValue, Entity newValue) {
    auto data = GetPropertyBinding(entity);

    auto oldEntityIndex = GetHandleIndex(oldValue);
    auto newEntityIndex = GetHandleIndex(newValue);

    if(IsEntityValid(oldValue)) {
        EntitySourceBindings.Insert(oldEntityIndex);

        for (auto i = 0; i < EntitySourceBindings[oldEntityIndex].size(); ++i) {
            if(EntitySourceBindings[oldEntityIndex][i] == entity) {
                EntitySourceBindings[oldEntityIndex].erase(EntitySourceBindings[oldEntityIndex].begin() + i);
                UnsubscribeComponentChanged(oldValue, OnSourceEntityDataChanged);
                break;
            }
        }
    }

    if(IsEntityValid(newValue)) {
        EntitySourceBindings.Insert(newEntityIndex);
        EntitySourceBindings[newEntityIndex].push_back(entity);
        SubscribeComponentChanged(newValue, OnSourceEntityDataChanged);
    }
}

static void OnPropertyBindingDestinationEntityChanged(Entity entity, Entity oldValue, Entity newValue) {
    auto data = GetPropertyBinding(entity);

    auto oldEntityIndex = GetHandleIndex(oldValue);
    auto newEntityIndex = GetHandleIndex(newValue);

    if(IsEntityValid(oldValue)) {
        EntityDestinationBindings.Insert(oldEntityIndex);

        for (auto i = 0; i < EntityDestinationBindings[oldEntityIndex].size(); ++i) {
            if(EntityDestinationBindings[oldEntityIndex][i] == entity) {
                EntityDestinationBindings[oldEntityIndex].erase(EntityDestinationBindings[oldEntityIndex].begin() + i);
                UnsubscribeComponentChanged(oldValue, OnDestinationEntityDataChanged);
                break;
            }
        }
    }

    if(IsEntityValid(newValue)) {
        EntityDestinationBindings.Insert(newEntityIndex);
        EntityDestinationBindings[newEntityIndex].push_back(entity);
        SubscribeComponentChanged(newValue, OnDestinationEntityDataChanged);
    }
}

static void OnEntityDestroyed(Entity entity) {
    auto entityIndex = GetHandleIndex(entity);

    if(EntityDestinationBindings.IsValid(entityIndex)) {
        for(auto& binding : EntityDestinationBindings[entityIndex]) {
            UnsubscribeComponentChanged(entity, OnDestinationEntityDataChanged);
        }

        EntityDestinationBindings.Remove(entityIndex);
    }

    if(HasPropertyBinding(entity)) {
        OnPropertyBindingSourceEntityChanged(entity, GetPropertyBindingSourceEntity(entity), 0);
        OnPropertyBindingDestinationEntityChanged(entity, GetPropertyBindingDestinationEntity(entity), 0);
    }
}

DefineService(PropertyBinding)
    Subscribe(PropertyBindingSourceEntityChanged, OnPropertyBindingSourceEntityChanged)
    Subscribe(PropertyBindingDestinationEntityChanged, OnPropertyBindingDestinationEntityChanged)
    Subscribe(EntityDestroyed, OnEntityDestroyed)
EndService()