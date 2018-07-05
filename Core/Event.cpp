//
// Created by Kim on 28/04/2018.
//

#include <Core/NativeUtils.h>
#include <Core/Event.h>
#include "Vector.h"
#include "Entity.h"
#include "Function.h"
#include "Debug.h"
#include "Hierarchy.h"
#include "Property.h"

struct Event {
    Vector(HandlerFunctions, Entity, 32);
    Vector(EventArgumentCache, EventArgument*, 8);
};

struct EventArgument {
    u32 EventArgumentOffset;
    u8 EventArgumentType;
};

struct Subscription {
    Entity SubscriptionHandler, SubscriptionEvent;
};

LocalFunction(OnEventArgumentTypeChanged, void, Entity argument, Type oldType, Type newType) {
    auto data = GetEventArgument(argument);

    for(auto type = 0; type < TypeOf_MAX; ++type) {
        if(type == data->EventArgumentType) {
            data->EventArgumentType = type;
            return;
        }
    }

    data->EventArgumentType = TypeOf_unknown;
    Log(argument, LogSeverity_Error, "Type not supported as argument: %s", GetTypeName(newType));
}

void FireEventFast(Entity event, u32 numArguments, const u8* argumentTypeIndices, const void **argumentDataPtrs) {

    Verbose("Firing event %s ...", GetName(event));

    auto data = GetEvent(event);

    if(data) {
        for(auto i = 0; i < data->NumHandlerFunctions; ++i) {
            CallFunction(data->HandlerFunctions[i], NULL, numArguments, argumentTypeIndices, argumentDataPtrs);
        }
    }
}

void FireEventNative(Entity event, const void *eventArgsStruct) {
    Verbose("Firing event %s ...", GetName(event));

    const void *argumentPtrs[32];
    u8 argumentTypeIndices[32];

    auto eventData = GetEvent(event);

    if(eventData) {
        auto numArguments = eventData->NumEventArgumentCache;

        if(numArguments == 0) {
            __ForceArgumentParse(event);
            numArguments = eventData->NumEventArgumentCache;
        }

        Assert(event, numArguments < 32);

        for(auto i = 0; i < numArguments; ++i) {
            argumentPtrs[i] = ((const u8*)eventArgsStruct) + eventData->EventArgumentCache[i]->EventArgumentOffset;
            argumentTypeIndices[i] = eventData->EventArgumentCache[i]->EventArgumentType;
        }

        for(auto i = 0; i < eventData->NumHandlerFunctions; ++i) {
            CallFunction(eventData->HandlerFunctions[i], NULL, numArguments, argumentTypeIndices, argumentPtrs);
        }
    }
}

//ChildCache(Event, EventArgument, EventArgumentCache)

//ReferenceTracker(SubscriptionHandler, SubscriptionEvent, Event, HandlerFunctions)

LocalFunction(OnEventArgumentParentChanged, void, Entity context, Entity oldParent, Entity newParent) {
    static auto parentComponent = ComponentOf_Event();
    static auto childComponent = ComponentOf_EventArgument();
    if(HasComponent(context, childComponent)) {
        auto childData = GetEventArgument(context);
        if(HasComponent(oldParent, parentComponent)) {
            auto data = GetEvent(oldParent);
            for(auto i = 0; i < data->NumEventArgumentCache; ++i) {
                if(data->EventArgumentCache[i] == childData) {
                    VectorRemove(data, EventArgumentCache, i);
                    break;
                }
            }
        }
        if(HasComponent(newParent, parentComponent)) {
            auto data = GetEvent(newParent);
            VectorAdd(data, EventArgumentCache, childData);
        }
    }
}

LocalFunction(OnSubscriptionHandlerChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto reference = GetSubscriptionEvent (context);

    if(IsEntityValid(reference)) {
        AddComponent(reference, ComponentOf_Event ());
        auto data = GetEvent (reference);

        for(auto i = 0; i < data->NumHandlerFunctions; ++i) {
            if(data-> HandlerFunctions [i] == oldValue) {
                VectorRemove(data, HandlerFunctions, i);
                break;
            }
        }

        if(IsEntityValid(newValue)) {
            VectorAdd(data, HandlerFunctions, newValue);
        }
    }
}

LocalFunction(OnSubscriptionEventChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto tracked = GetSubscriptionHandler(context);

    if(IsEntityValid(oldValue)) {
        AddComponent(oldValue, ComponentOf_Event ());
        auto data = GetEvent (oldValue);

        for(auto i = 0; i < data->NumHandlerFunctions; ++i) {
            if(data-> HandlerFunctions [i] == tracked) {
                VectorRemove(data, HandlerFunctions, i);
                break;
            }
        }
    }

    if(IsEntityValid(newValue) && IsEntityValid(tracked)) {
        auto data = GetEvent(newValue);
        VectorAdd(data, HandlerFunctions, newValue);
    }
}

API_EXPORT void SetSubscriptionHandler(Entity entity, Entity value)  {
    static Entity prop = PropertyOf_SubscriptionHandler ();
    auto oldValue = GetSubscriptionHandler(entity);
    SetPropertyValue(prop, entity, &value);

    OnSubscriptionHandlerChanged(entity, oldValue, value);
}

API_EXPORT void SetSubscriptionEvent(Entity entity, Entity value)  {
    static Entity prop = PropertyOf_SubscriptionEvent ();
    auto oldValue = GetSubscriptionEvent(entity);
    SetPropertyValue(prop, entity, &value);

    OnSubscriptionEventChanged(entity, oldValue, value);
}

BeginUnit(Event)
    BeginComponent(Event)
    EndComponent()

    BeginComponent(EventArgument)
        RegisterProperty(u32, EventArgumentOffset)
        RegisterProperty(u8, EventArgumentType)
    EndComponent()

    BeginComponent(Subscription)
        RegisterProperty(Entity, SubscriptionHandler)
        RegisterProperty(Entity, SubscriptionEvent)
    EndComponent()

    __ForceArgumentParse(EventOf_SubscriptionEventChanged());
    __ForceArgumentParse(EventOf_SubscriptionHandlerChanged());

    RegisterFunction(OnSubscriptionHandlerChanged)\
    RegisterFunction(OnSubscriptionEventChanged)\

    RegisterChildCache(EventArgument)

    RegisterSubscription(EventArgumentTypeChanged, OnEventArgumentTypeChanged)
EndUnit()

void __InitializeEvent() {
    auto component = ComponentOf_EventArgument();
    __Property(PropertyOf_EventArgumentOffset(), offsetof(EventArgument, EventArgumentOffset), sizeof(EventArgument::EventArgumentOffset), TypeOf_u32,  component);
    __Property(PropertyOf_EventArgumentType(), offsetof(EventArgument, EventArgumentType), sizeof(EventArgument::EventArgumentType), TypeOf_Type,  component);
}

void __ForceArgumentParse(Entity eventEntity) {
    auto data = GetEvent(eventEntity);

    VectorClear(data, EventArgumentCache);

    for_children(child, eventEntity) {
        if(HasComponent(child, ComponentOf_EventArgument())) {
            auto childData = GetEventArgument(child);
            VectorAdd(data, EventArgumentCache, childData);
        }
    }
}

