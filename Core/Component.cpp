//
// Created by Kim on 03/06/2018.
//

#include <Core/Component.h>
#include <Core/Function.h>
#include <Core/Entity.h>
#include <Core/Event.h>
#include <Core/Pool.h>
#include "Identification.h"
#include "Component.h"


static bool __isComponentInitialized = false;

#define Verbose_Component "component"

struct ComponentTypeData {
    Pool DataBuffer;
    Vector(EntityComponentIndices, u32, 128);
};

static eastl::fixed_vector<ComponentTypeData, 512> typeData;

API_EXPORT ComponentTypeData* ComponentDataList[USHRT_MAX];

struct Extension {
    Entity ExtensionComponent, ExtensionExtenderComponent;
    bool ExtensionDisabled;
};

struct Base {
    Entity BaseComponent;
};

struct Component {
    u16 ComponentDataIndex;
    u16 ComponentSize;
    bool ComponentExplicitSize;
};

API_EXPORT ComponentTypeData *GetComponentType(Entity component) {
    auto entityIndex = GetEntityIndex(component);
    if(!ComponentDataList[entityIndex]) {
        typeData.push_back();
        ComponentDataList[entityIndex] = &typeData[typeData.size() - 1];
    }
    return ComponentDataList[entityIndex];
}

API_EXPORT u32 GetComponentMax (Entity component) {
    return GetComponentType(component)->DataBuffer.End();
}

API_EXPORT Entity GetComponentEntity(Entity component, u32 index) {
    auto componentData = GetComponentType(component);

    auto entity = *(Entity*)componentData->DataBuffer[index];
    return entity;
}

API_EXPORT u32 GetComponentIndex(Entity component, Entity entity) {
    auto componentData = GetComponentType(component);
    if(!componentData) {
        Error(0, "Cannot get index of invalid component");
        return InvalidIndex;
    }

    auto entityIndex = GetEntityIndex(entity);

    if(componentData->EntityComponentIndices.Count <= entityIndex) {
        return InvalidIndex;
    }

    return GetVector(componentData->EntityComponentIndices)[entityIndex];
}

API_EXPORT bool HasComponent (Entity entity, Entity component) {
    return GetComponentIndex(component, entity) != InvalidIndex;
}

API_EXPORT char * GetComponentBytes(Entity component, u32 index) {
    if(index == InvalidIndex || !IsEntityValid(component)) return NULL;

    auto componentData = GetComponentType(component);
    return componentData->DataBuffer[index] + sizeof(Entity)*2;
}


API_EXPORT char *GetComponentData(u32 componentEntityIndex, Entity entity) {
    auto entityIndex = GetEntityIndex(entity);
    auto componentData = ComponentDataList[componentEntityIndex];

    auto count = componentData->EntityComponentIndices.Count;
    if(count <= entityIndex) {
        auto newCount = entityIndex + 1;
        SetVectorAmount(componentData->EntityComponentIndices, newCount);
        memset(&GetVector(componentData->EntityComponentIndices)[count], 0xff, sizeof(u32) * (newCount - count));
    }

    auto index = GetVector(componentData->EntityComponentIndices)[entityIndex];

    if(index == InvalidIndex) return NULL;

    return componentData->DataBuffer[index] + sizeof(Entity)*2;
}


API_EXPORT bool AddComponent (Entity entity, Entity component) {
    if(!HasComponent (entity, component)) {
        if(!IsEntityValid(entity)) {
            Log(0, LogSeverity_Error, "Invalid Entity when adding component %s: %s", GetDebugName(component), GetDebugName(entity));
            return false;
        }

        if(!IsEntityValid(component)) {
            Log(0, LogSeverity_Error, "Cannot add invalid Component");
            return false;
        }

        auto componentData = GetComponentType(component);

        auto componentIndex = componentData->DataBuffer.Add();
        auto entityIndex = GetEntityIndex(entity);

        auto count = componentData->EntityComponentIndices.Count;
        if(count <= entityIndex) {
            auto newCount = entityIndex + 1;
            SetVectorAmount(componentData->EntityComponentIndices, newCount);
            memset(&GetVector(componentData->EntityComponentIndices)[count], 0xff, sizeof(u32) * (newCount - count));
        }

        GetVector(componentData->EntityComponentIndices)[entityIndex] = componentIndex;
        *(Entity*)componentData->DataBuffer[componentIndex] = entity;

        auto dataSize = GetComponentSize(component);
        if(dataSize > 0) {
            memset(componentData->DataBuffer[componentIndex] + sizeof(Entity)*2, 0, dataSize);
        }

        if(__isComponentInitialized) {
            for_children(property, Properties, component, {
                auto propertyData = GetPropertyData(property);
                if(GetPropertyKind(property) != PropertyKind_Child) continue;

                auto child = CreateEntity();
                auto parentUuid = GetUuid(entity);
                auto propertyName = GetName(property);
                char *buffer = (char*)alloca(strlen(parentUuid) + strlen(propertyName) + 2);
                sprintf(buffer, "%s.%s", parentUuid, propertyName);
                SetUuid(child, buffer);
                SetName(child, propertyName);
                auto offset = GetPropertyOffset(property);

                *(Entity*)(componentData->DataBuffer[componentIndex] + sizeof(Entity)*2 + offset) = child;
                SetOwner(child, entity, property);

                AddComponent(child, GetPropertyChildComponent(property));
            });

            for_children(base, Bases, component, {
                AddComponent(entity, GetBaseComponent(base));
            });


            Variant arguments[] = {MakeVariant(Entity, component), MakeVariant(Entity, entity)};
			FireEventFast(EventOf_EntityComponentAdded(), 2, arguments);

            for_entity(extension, extensionData, Extension, {
			    if(extensionData->ExtensionComponent == component) {
                    if(!extensionData->ExtensionDisabled) {
                        AddComponent(entity, extensionData->ExtensionExtenderComponent);
                    }
			    }
			});
        }

        Verbose(Verbose_Component, "Component %s has been added to Entity %s.", GetDebugName(component), GetDebugName(entity));

        return true;
    }
    return false;
}

API_EXPORT bool RemoveComponent (Entity entity, Entity component) {
    if(!IsEntityValid(entity)) {
        Log(0, LogSeverity_Error, "Invalid Entity when removing component %s: %s", GetDebugName(component), GetDebugName(entity));
        return false;
    }

    if(!IsEntityValid(component)) {
        Log(0, LogSeverity_Error, "Cannot remove invalid Component");
        return false;
    }

    if(HasComponent(entity, component)) {
        for_entity(extension, extensionData, Extension, {
            if(extensionData->ExtensionComponent == component) {
                if(!extensionData->ExtensionDisabled) {
                    RemoveComponent(entity, extensionData->ExtensionExtenderComponent);
                }
            }
        });

		Type types[] = { TypeOf_Entity, TypeOf_Entity };
		const Variant values[] = { MakeVariant(Entity, component), MakeVariant(Entity, entity) };
		FireEventFast(EventOf_EntityComponentRemoved(), 2, values);

        if(HasComponent(entity, component)) {
            auto componentData = GetComponentType(component);
            auto deletionEntityIndex = GetEntityIndex(entity);
            auto deletionComponentIndex = GetVector(componentData->EntityComponentIndices)[deletionEntityIndex];

            u32 count = 0;
            auto properties = GetProperties(component, &count);
            for(auto i = 0; i < count; ++i) {
                auto property = properties[i];
                auto kind = GetPropertyKind(property);

                if(kind == PropertyKind_Child) {
                    auto childEntity = GetPropertyValue(property, entity);
                    DestroyEntity(childEntity.as_Entity);
                } else if(kind == PropertyKind_Array) {
                    while(GetArrayPropertyCount(property, entity)) {
                        RemoveArrayPropertyElement(property, entity, 0);
                    }
                } else {
                    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

                    SetPropertyValue(property, entity, Variant_Empty); // Reset value to default before removal
                }
            }

            GetVector(componentData->EntityComponentIndices)[deletionEntityIndex] = InvalidIndex;

            memset(componentData->DataBuffer[deletionComponentIndex], 0, componentData->DataBuffer.GetElementSize());

            componentData->DataBuffer.Remove(deletionComponentIndex);
        }

        Verbose(Verbose_Component, "Component %s has been removed from Entity %s.", GetDebugName(component), GetDebugName(entity));

        return true;
    }
    return false;
}


API_EXPORT u32 GetNumComponentPages(Entity component) {
    auto componentData = GetComponentType(component);
    return componentData->DataBuffer.GetNumPages();
}

API_EXPORT char *GetComponentPage(Entity component, u32 index, u32 *elementStride) {
    auto componentData = GetComponentType(component);
    *elementStride = componentData->DataBuffer.GetBlockSize();
    return componentData->DataBuffer.GetPage(index);
}


API_EXPORT u32 GetNextComponent(Entity component, u32 index, void **dataPtr, Entity *entity) {
    ++index;

    auto componentData = GetComponentType(component);
    auto amount = componentData->DataBuffer.End();
    while(!componentData->DataBuffer.IsValid(index)) {
        ++index;

        if(index >= amount) {
            if(dataPtr) *dataPtr = 0;
            if(entity) *entity = 0;
            return InvalidIndex;
        }
    }

    auto entryData = componentData->DataBuffer[index];
    if(dataPtr) *dataPtr = entryData + sizeof(Entity)*2;
    if(entity) *entity = *(Entity*)entryData;

    return index;
}

API_EXPORT void SetComponentSize(Entity entity, u16 value) {
    auto data = GetComponentData(entity);
    Variant oldValue, newValue;
    if(data) {
        oldValue = MakeVariant(u16, data->ComponentSize);
        data->ComponentSize = value;
    }

    newValue = MakeVariant(u16, value);

    auto componentData = GetComponentType(entity);
    componentData->DataBuffer.SetElementSize(value + sizeof(Entity)*2);

    if(__isComponentInitialized) {
        EmitChangedEvent(entity, PropertyOf_ComponentSize(), GetPropertyData(PropertyOf_ComponentSize()), oldValue, MakeVariant(u16, value));
    }
}

API_EXPORT u16 GetComponentSize(Entity entity) {
    if(HasComponent(entity, ComponentOf_Component())) {
        return GetComponentData(entity)->ComponentSize;
    }

    return 0;
}

__PropertyCoreImpl(bool, ComponentExplicitSize, Component)
__ArrayPropertyCoreImpl(Property, Properties, Component)
__ArrayPropertyCoreImpl(Base, Bases, Component)

LocalFunction(OnEntityDestroyed, void, Entity entity) {
    for_entity(component, data, Component, {
        RemoveComponent(entity, component);
    });
}

static void RemoveExtensions(Entity component, Entity extensionComponent) {
    Entity entity = 0;
    void *data = 0;
    for(u32 i = GetNextComponent(component, InvalidIndex, &data, &entity);
        i != InvalidIndex;
        i = GetNextComponent(component, i, &data, &entity)) {
        RemoveComponent(entity, extensionComponent);
    }
}

static void AddExtensions(Entity component, Entity extensionComponent) {
    Entity entity = 0;
    void *data = 0;
    for(u32 i = GetNextComponent(component, InvalidIndex, &data, &entity);
        i != InvalidIndex;
        i = GetNextComponent(component, i, &data, &entity)) {
        AddComponent(entity, extensionComponent);
    }
}

LocalFunction(OnExtensionDisabledChanged,
        void,
        Entity extension,
        bool oldValue,
        bool newValue) {
    if(newValue) {
        AddExtensions(GetExtensionComponent(extension), GetExtensionExtenderComponent(extension));
    }

    if(!newValue) {
        RemoveExtensions(GetExtensionComponent(extension), GetExtensionExtenderComponent(extension));
    }
}

LocalFunction(OnExtensionComponentChanged,
        void,
        Entity extension,
        Entity oldValue,
        Entity newValue) {
    if(!GetExtensionDisabled(extension)) {
        RemoveExtensions(oldValue, GetExtensionExtenderComponent(extension));
        AddExtensions(newValue, GetExtensionExtenderComponent(extension));
    }
}

LocalFunction(OnExtensionExtenderComponentChanged,
        void,
        Entity extension,
        Entity oldValue,
        Entity newValue) {
    if(!GetExtensionDisabled(extension)) {
        RemoveExtensions(GetExtensionComponent(extension), oldValue);
        AddExtensions(GetExtensionComponent(extension), newValue);
    }
}

BeginUnit(Component)
    BeginComponent(Base)
        RegisterReferenceProperty(Component, BaseComponent)
    EndComponent()

    BeginComponent(Component)
        RegisterArrayProperty(Property, Properties)
        RegisterArrayProperty(Base, Bases)
        RegisterPropertyReadOnly(u16, ComponentSize)
        RegisterPropertyReadOnly(bool, ComponentExplicitSize)
    EndComponent()

    BeginComponent(Extension)
        RegisterReferenceProperty(Component, ExtensionComponent)
        RegisterReferenceProperty(Component, ExtensionExtenderComponent)
        RegisterProperty(bool, ExtensionDisabled)
    EndComponent()

    RegisterEvent(EntityComponentAdded)
    RegisterEvent(EntityComponentRemoved)

    __isComponentInitialized = true;

    RegisterSubscription(EventOf_EntityDestroyed(), OnEntityDestroyed, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionDisabled()), OnExtensionDisabledChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionComponent()), OnExtensionComponentChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionExtenderComponent()), OnExtensionExtenderComponentChanged, 0)
EndUnit()


void __InitializeComponent() {
    auto component = ComponentOf_Component();
    AddComponent(component, ComponentOf_Component());
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Properties(), InvalidIndex, 0, TypeOf_Entity, component, ComponentOf_Property(), PropertyKind_Array);
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16, component, 0, PropertyKind_Value);
}

void __PreInitialize() {
    memset(ComponentDataList, 0, sizeof(ComponentDataList));
}
