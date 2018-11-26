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

    data->ReplicationLastChangeFrame = GetAppLoopFrame(AppLoopOf_Networking());

    for_entity(property, propertyData, Property, {
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
                Entity child = GetPropertyValue(property, context).as_Entity;
                if(IsEntityValid(child)) {
                    AddComponent(child, ComponentOf_Replication());
                    Replicate(child);
                }
            }
        }
    });
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
        data->ReplicationLastChangeFrame = GetAppLoopFrame(AppLoopOf_Networking());
    }
}

LocalFunction(OnComponentRemoved, void, Entity component, Entity context) {
    if(component == ComponentOf_Replication() || component == ComponentOf_Ownership() || component == ComponentOf_Identification()) return;

    auto data = GetReplicationData(context);
    if(data) {
        data->ReplicationLastChangeFrame = GetAppLoopFrame(AppLoopOf_Networking());
    }
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    auto data = GetReplicationData(entity);
    if(data) {
        data->ReplicationLastChangeFrame = GetAppLoopFrame(AppLoopOf_Networking());
    }
}

BeginUnit(Replication)
    BeginComponent(Replication)
        RegisterProperty(u64, ReplicationLastChangeFrame)
    EndComponent()

    RegisterGenericPropertyChangedListener(OnPropertyChanged);
    RegisterSubscription(EventOf_EntityComponentAdded(), OnComponentAdded, 0)
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnComponentRemoved, 0)
EndUnit()
