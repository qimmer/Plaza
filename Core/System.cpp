//
// Created by Kim on 25-03-2019.
//

#include "System.h"
#include "Strings.h"

struct CompareSystemOrder {
    bool operator()(const Entity& a, const Entity& b) const {
        return GetSystem(a).SystemOrder < GetSystem(b).SystemOrder;
    }
};

struct CompareOwnerLevel {
    bool operator()(const eastl::pair<Entity, Entity>& a, const eastl::pair<Entity, Entity>& b) const {
        return GetOwnership(a.first).OwnerLevel < GetOwnership(b.first).OwnerLevel || a.second < b.second;
    }
};

eastl::map<Entity, eastl::set<Entity>> ComponentSystemsMap;
eastl::fixed_map<Entity, eastl::map<eastl::pair<Entity, Entity>, u32, CompareOwnerLevel>, 256, true, CompareSystemOrder> SystemChanges;

// Use v4f as element type to 16-byte-align all oldValue component data
eastl::vector<v4f> OldValueBuffer;

typedef void(*GenericSystemPtr)(Entity entity, Entity component, const void *value, const void *oldValue);
typedef void(*SystemPtr)(Entity entity, const void *value, const void *oldValue);

static void NotifySystem(Entity system, const Component& componentData, u32 componentIndex, Entity entity, Entity component, const void *oldData, const void *newData) {
    auto data = GetSystem(system);

    // If system is not deferred, call systems immediately
    if(!data.SystemDeferred) {
        ((GenericSystemPtr)data.SystemFunction)(entity, component, newData, oldData);
    } else {
        // Make sure we store old value and defer the system call
        auto& map = SystemChanges[system];
        auto it = map.find({entity, component});
        if(it == map.end()) {
            // Only reserve and copy old data if this is the first change. Subsequent changes will use first old value and current value as newValue
            auto offset = OldValueBuffer.size();
            OldValueBuffer.resize(OldValueBuffer.size() + (componentData.ComponentSize / sizeof(v4f)) + 1);

            memcpy(OldValueBuffer.data() + offset, oldData, componentData.ComponentSize);

            map[{entity, component}] = offset;
        }
    }
}

API_EXPORT void NotifyChange(Entity entity, Entity component, const void *oldData, const void *newData) {
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto componentIndex = GetComponentIndexByIndex(componentInfoIndex, entity);

    auto componentData = GetComponent(component);

    // First, call all generic systems
    auto& systems = ComponentSystemsMap[0];
    for(auto system : systems) {
        NotifySystem(system, componentData, componentIndex, entity, component, oldData, newData);
    }

    systems = ComponentSystemsMap[component];
    for(auto system : systems) {
        NotifySystem(system, componentData, componentIndex, entity, component, oldData, newData);
    }
}

API_EXPORT void ProcessSystems() {
    while(SystemChanges.size()) {
        auto state = SystemChanges;
        auto oldValues = OldValueBuffer;

        // Prepare for new change notifications
        SystemChanges.clear();
        oldValues.clear();

        // Iterate every deferred system that has pending notifications
        for(auto& it : state) {
            auto systemData = GetSystem(it.first);

            auto componentInfoIndex = GetComponentIndexByIndex(0, systemData.SystemComponent);

            // Invoke system on every change
            for(auto& change : it.second) {
                auto componentIndex = GetComponentIndexByIndex(componentInfoIndex, change.first.first);

                ((GenericSystemPtr)systemData.SystemFunction)(change.first.first, change.first.second, GetComponentInstanceData(componentInfoIndex, componentIndex), &oldValues[change.second]);
            }
        }

        // Free up temporary memory buffer
        TempFree();
    }

    CleanupStrings();
}

BeginUnit(System)
    BeginComponent(System)
        RegisterReferenceProperty(Component, SystemComponent)
        RegisterProperty(NativePtr, SystemFunction)
        RegisterProperty(float, SystemOrder)
        RegisterProperty(bool, SystemDeferred)
    EndComponent()
EndUnit()