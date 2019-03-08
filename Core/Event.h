#ifndef PLAZA_DELEGATE_H
#define PLAZA_DELEGATE_H

#include <Core/NativeUtils.h>
#include <Core/Variant.h>

struct Subscription {
    Entity SubscriptionHandler, SubscriptionEvent, SubscriptionSender;
};

struct Event {
};

struct EventArgument {
    u32 EventArgumentOffset;
    u8 EventArgumentType;
};

typedef Vector<Subscription, 4> SubscriptionCacheVector;

SubscriptionCacheVector& GetSubscriptionCache(Entity event);

#include <Core/Component.h>

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

void FireEventFast(
    Entity event,
    u32 numArguments,
    const Variant *arguments
);

void __InitializeEvent();
void __ForceArgumentParse(Entity eventEntity);

#endif