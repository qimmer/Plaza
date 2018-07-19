#ifndef PLAZA_DELEGATE_H
#define PLAZA_DELEGATE_H

#include <Core/NativeUtils.h>

Unit(Event)
    Component(EventArgument)
        __PropertyCore(EventArgument, Type, EventArgumentType)
        __PropertyCore(EventArgument, u32, EventArgumentOffset)

    Component(Event)
        __ArrayPropertyCore(EventArgument, EventArguments)

    Component(Subscription)
        __PropertyCore(Subscription, Entity, SubscriptionEvent)
        __PropertyCore(Subscription, Entity, SubscriptionSender)
        __PropertyCore(Subscription, Entity, SubscriptionHandler)

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