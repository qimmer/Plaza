//
// Created by Kim on 25-08-2018.
//

#include <Foundation/AppLoop.h>
#include <Core/Identification.h>
#include "Replication.h"
#include "NetworkingModule.h"

struct Replication {
    u64 ReplicationLastChangeFrame;
};

static void Replicate(Entity context) {
    auto data = GetReplicationData(context);

    data->ReplicationLastChangeFrame = GetAppLoopFrame(GetReplicationAppLoop(ModuleOf_Networking()));

    for_entity(property, propertyData, Property) {
        auto component = GetOwner(property);
        if(HasComponent(context, component)) {
            auto kind = GetPropertyKind(property);

            if(kind == PropertyKind_Array) {
                for(auto i = 0; i < GetArrayPropertyCount(property, context); ++i) {
                    auto element = GetArrayPropertyElement(property, context, i);
                    AddComponent(element, ComponentOf_Replication());

                    Replicate(element);
                }
            }

            if(kind == PropertyKind_Child) {
                Entity child = 0;
                GetPropertyValue(property, context, &child);
                if(IsEntityValid(child)) {
                    AddComponent(child, ComponentOf_Replication());
                    Replicate(child);
                }
            }
        }
    }
}

LocalFunction(OnComponentAdded, void, Entity component, Entity context) {
    if(component == ComponentOf_Ownership() || component == ComponentOf_Identification()) return;

    auto owner = GetOwner(context);
    if(HasComponent(owner, ComponentOf_Replication())) {
        AddComponent(context, ComponentOf_Replication());
    }

    if(component == ComponentOf_Replication() && !HasComponent(owner, ComponentOf_Replication())) {
        Replicate(context);
    }

    auto data = GetReplicationData(context);
    if(data) {
        data->ReplicationLastChangeFrame = GetAppLoopFrame(GetReplicationAppLoop(ModuleOf_Networking()));
    }
}

LocalFunction(OnComponentRemoved, void, Entity component, Entity context) {
    if(component == ComponentOf_Replication() || component == ComponentOf_Ownership() || component == ComponentOf_Identification()) return;

    auto data = GetReplicationData(context);
    if(data) {
        data->ReplicationLastChangeFrame = GetAppLoopFrame(GetReplicationAppLoop(ModuleOf_Networking()));
    }
}

LocalFunction(OnPropertyChanged, void, Entity property, Entity entity) {
    auto data = GetReplicationData(entity);
    if(data) {
        data->ReplicationLastChangeFrame = GetAppLoopFrame(GetReplicationAppLoop(ModuleOf_Networking()));
    }
}

BeginUnit(Replication)
    BeginComponent(Replication)
        RegisterProperty(u64, ReplicationLastChangeFrame)
    EndComponent()

    RegisterSubscription(PropertyChanged, OnPropertyChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnComponentAdded, 0)
    RegisterSubscription(EntityComponentRemoved, OnComponentRemoved, 0)
EndUnit()
