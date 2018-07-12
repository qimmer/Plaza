#ifndef PLAZA_DELEGATE_H
#define PLAZA_DELEGATE_H

#include <Core/NativeUtils.h>

Unit(Event)
    Component(Event)

    Component(EventArgument)
        Property(Type, EventArgumentType)
        Property(u32, EventArgumentOffset)

    Component(Subscription)
        __PropertyCore(Entity, SubscriptionEvent)
        __PropertyCore(Entity, SubscriptionSender)
        __PropertyCore(Entity, SubscriptionHandler)

void SetEventArgsByDecl(Entity entity, StringRef decl);
void FireEvent(Entity event, Entity context, ...);

void FireEventFast(
    Entity event,
    u32 numArguments,
    const u8* argumentTypeIndices,
    const void **argumentDataPtrs
);

void __InitializeEvent();
void __ForceArgumentParse(Entity eventEntity);

#endif