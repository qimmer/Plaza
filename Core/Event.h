#ifndef PLAZA_DELEGATE_H
#define PLAZA_DELEGATE_H

#include <Core/NativeUtils.h>

void FireEventFast(
    Entity event,
    u32 numArguments,
    const u8* argumentTypeIndices,
    const void **argumentDataPtrs
);

void FireEventNative(Entity event, const void *eventArgsStruct);

Unit(Event)

Component(Event)

Component(EventArgument)
    Property(Type, EventArgumentType)
    Property(u32, EventArgumentOffset)

Component(Subscription)
    __PropertyCore(Entity, SubscriptionEvent)
    __PropertyCore(Entity, SubscriptionHandler)

void __InitializeEvent();
void __ForceArgumentParse(Entity eventEntity);

#endif