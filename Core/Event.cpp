//
// Created by Kim on 28/04/2018.
//

#include <Core/NativeUtils.h>
#include <Core/Event.h>
#include <Core/Vector.h>
#include <Core/Entity.h>
#include <Core/Function.h>
#include <Core/Debug.h>
#include <Core/Node.h>
#include <Core/Property.h>
#include <Core/Math.h>

#include <stdarg.h>

struct Subscription {
    Entity SubscriptionHandler, SubscriptionEvent, SubscriptionSender;
};

struct Event {
    Vector(SubscriptionCache, Subscription, 128);
    Vector(EventArgumentCache, EventArgument*, 8);
};

struct EventArgument {
    u32 EventArgumentOffset;
    u8 EventArgumentType;
};

LocalFunction(OnEventArgumentTypeChanged, void, Entity argument, Type oldType, Type newType) {
    auto data = GetEventArgumentData(argument);

    for(auto type = 0; type < TypeOf_MAX; ++type) {
        if(type == data->EventArgumentType) {
            data->EventArgumentType = type;
            return;
        }
    }

    data->EventArgumentType = TypeOf_unknown;
    Log(argument, LogSeverity_Error, "Type not supported as argument: %s", GetTypeName(newType));
}

LocalFunction(OnEventArgumentParentChanged, void, Entity context, Entity oldParent, Entity newParent) {
    static auto parentComponent = ComponentOf_Event();
    static auto childComponent = ComponentOf_EventArgument();
    if(HasComponent(context, childComponent)) {
        auto childData = GetEventArgumentData(context);
        if(HasComponent(oldParent, parentComponent)) {
            auto data = GetEventData(oldParent);
            for(auto i = 0; i < data->NumEventArgumentCache; ++i) {
                if(data->EventArgumentCache[i] == childData) {
                    VectorRemove(data, EventArgumentCache, i);
                    break;
                }
            }
        }
        if(HasComponent(newParent, parentComponent)) {
            auto data = GetEventData(newParent);
            VectorAdd(data, EventArgumentCache, childData);
        }
    }
}

LocalFunction(OnSubscriptionHandlerChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto subscription = GetSubscriptionData (context);

    if(IsEntityValid(subscription->SubscriptionEvent)) {
        AddComponent(subscription->SubscriptionEvent, ComponentOf_Event ());
        auto event = GetEventData (subscription->SubscriptionEvent);

        for(auto i = 0; i < event->NumSubscriptionCache; ++i) {
            if(event-> SubscriptionCache [i].SubscriptionHandler == oldValue) {
                VectorRemove(event, SubscriptionCache, i);
                break;
            }
        }

        if(IsEntityValid(newValue)) {
            VectorAdd(event, SubscriptionCache, *subscription);
        }
    }
}

LocalFunction(OnSubscriptionSenderChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto subscription = GetSubscriptionData (context);

    if(IsEntityValid(subscription->SubscriptionEvent)) {
        AddComponent(subscription->SubscriptionEvent, ComponentOf_Event ());
        auto event = GetEventData (subscription->SubscriptionEvent);

        for(auto i = 0; i < event->NumSubscriptionCache; ++i) {
            if(event-> SubscriptionCache [i].SubscriptionSender == oldValue) {
                event-> SubscriptionCache [i].SubscriptionSender = newValue;
                break;
            }
        }
    }
}

LocalFunction(OnSubscriptionEventChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto subscription = GetSubscriptionData (context);

    if(IsEntityValid(oldValue)) {
        AddComponent(oldValue, ComponentOf_Event ());
        auto data = GetEventData (oldValue);

        for(auto i = 0; i < data->NumSubscriptionCache; ++i) {
            if(data-> SubscriptionCache [i].SubscriptionEvent == subscription->SubscriptionEvent) {
                VectorRemove(data, SubscriptionCache, i);
                break;
            }
        }
    }

    if(IsEntityValid(newValue) && IsEntityValid(subscription->SubscriptionEvent)) {
        AddComponent(newValue, ComponentOf_Event());
        auto data = GetEventData(newValue);
        VectorAdd(data, SubscriptionCache, *subscription);
    }
}

void __InitializeEvent() {
    auto component = ComponentOf_EventArgument();
    __Property(PropertyOf_EventArgumentOffset(), offsetof(EventArgument, EventArgumentOffset), sizeof(EventArgument::EventArgumentOffset), TypeOf_u32,  component);
    __Property(PropertyOf_EventArgumentType(), offsetof(EventArgument, EventArgumentType), sizeof(EventArgument::EventArgumentType), TypeOf_Type,  component);
}

void __ForceArgumentParse(Entity eventEntity) {
    auto data = GetEventData(eventEntity);

    VectorClear(data, EventArgumentCache);

    for_children(child, eventEntity) {
        if(HasComponent(child, ComponentOf_EventArgument())) {
            auto childData = GetEventArgumentData(child);
            VectorAdd(data, EventArgumentCache, childData);
        }
    }
}

API_EXPORT void FireEventFast(Entity event, u32 numArguments, const u8* argumentTypeIndices, const void **argumentDataPtrs) {

    Verbose("Firing event %s ...", GetName(event));

    auto data = GetEventData(event);

    if(data) {
        /*if(numArguments != (data->NumEventArgumentCache + 1)) {
            Log(event, LogSeverity_Error, "FireEventFast failed. Incorrect number of arguments provided.");
            return;
        }*/

        auto sender = *(const Entity*)argumentDataPtrs[0];
        for(auto i = 0; i < data->NumSubscriptionCache; ++i) {
            auto subscription = &data->SubscriptionCache[i];
            if(!subscription->SubscriptionSender || !sender || subscription->SubscriptionSender == sender) {
                CallFunction(subscription->SubscriptionHandler, NULL, numArguments, argumentTypeIndices, argumentDataPtrs);
            }
        }
    }
}

API_EXPORT void FireEvent(Entity event, Entity sender, ...) {

#define __HANDLE_VA_ARG(TYPE, VA_TYPE) \
    case TypeOf_ ## TYPE :\
        *(TYPE*)argPtr = va_arg(vl, VA_TYPE);\
        argumentPtrs[i+1] = argPtr;\
        argPtr += sizeof(TYPE);\
        break;

    Verbose("Firing event %s ...", GetName(event));

    char argBuffer[256];
    char *argPtr = argBuffer;

    const void *argumentPtrs[32];
    u8 argumentTypeIndices[32];

    auto eventData = GetEventData(event);

    if(eventData) {
        size_t numArguments = eventData->NumEventArgumentCache;

        if(numArguments == 0) {
            __ForceArgumentParse(event);
            numArguments = eventData->NumEventArgumentCache;
        }

        va_list vl;
        va_start(vl, numArguments);

        Assert(event, numArguments < 32);

        argumentTypeIndices[0] = TypeOf_Entity;
        argumentPtrs[0] = &sender;

        for(auto i = 0; i < numArguments; ++i) {
            auto type = eventData->EventArgumentCache[i]->EventArgumentType;

            switch(type) {
                __HANDLE_VA_ARG(bool, int)
                __HANDLE_VA_ARG(u8, int)
                __HANDLE_VA_ARG(u16, int)
                __HANDLE_VA_ARG(u32, u32)
                __HANDLE_VA_ARG(u64, u64)
                __HANDLE_VA_ARG(s8, int)
                __HANDLE_VA_ARG(s16, int)
                __HANDLE_VA_ARG(s32, s32)
                __HANDLE_VA_ARG(s64, s64)
                __HANDLE_VA_ARG(float, double)
                __HANDLE_VA_ARG(double, double)
                __HANDLE_VA_ARG(StringRef, StringRef)
                __HANDLE_VA_ARG(Entity, Entity)
                __HANDLE_VA_ARG(v2i, v2i)
                __HANDLE_VA_ARG(v3i, v3i)
                __HANDLE_VA_ARG(v4i, v4i)
                __HANDLE_VA_ARG(v2f, v2f)
                __HANDLE_VA_ARG(v3f, v3f)
                __HANDLE_VA_ARG(v4f, v4f)
                default:
                    Log(0, LogSeverity_Error, "Error firing event. Argument type '%s' is unsupported.", GetTypeName(type));
                    return;
                    break;
            }

            argumentTypeIndices[i+1] = type;
        }

        for(auto i = 0; i < eventData->NumSubscriptionCache; ++i) {
            auto subscription = &eventData->SubscriptionCache[i];
            if(!sender) {
                argumentPtrs[0] = &subscription->SubscriptionSender;
            }
            if(!subscription->SubscriptionSender || !sender || subscription->SubscriptionSender == sender) {
                CallFunction(subscription->SubscriptionHandler, NULL, numArguments + 1, argumentTypeIndices, argumentPtrs);
            }
        }

        va_end(vl);
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

API_EXPORT void SetSubscriptionSender(Entity entity, Entity value)  {
    static Entity prop = PropertyOf_SubscriptionSender ();
    auto oldValue = GetSubscriptionSender(entity);
    SetPropertyValue(prop, entity, &value);

    OnSubscriptionSenderChanged(entity, oldValue, value);
}

API_EXPORT void SetEventArgsByDecl(Entity f, StringRef arguments) {
    auto len = strlen(arguments);
    auto offset = 0;
    auto byteOffset = 0;

    char argumentSplits[PATH_MAX];
    strcpy(argumentSplits, arguments);

    while(offset < len) {
        auto argument = &argumentSplits[0] + offset;
        auto nextComma = strchr(&argumentSplits[0] + offset, ',');
        if(!nextComma) nextComma = &argumentSplits[0] + len;
        auto argumentLength = nextComma - argument;
        offset += argumentLength + 1;
        *nextComma = '\0';

        auto name = strrchr(argument, ' ');
        Assert(f, name);
        *name = '\0';
        name++;

        auto typeSignature = argument;
        auto typeSignatureEnd = name - 2;
        // Trim space
        while(isspace(*typeSignature)) typeSignature++;
        while(isspace(*typeSignatureEnd) && typeSignatureEnd > typeSignature) {
            *typeSignatureEnd = '\0';
            typeSignatureEnd--;
        }

        auto argumentEntity = CreateEntity();
        SetParent(argumentEntity, f);
        SetName(argumentEntity, name);

        auto type = 0;
        for(auto i = 0; i < TypeOf_MAX; ++i) {
            if(strcmp(typeSignature, GetTypeName(i)) == 0) {
                type = i;
                break;
            }
        }

        if(!type) {
            Log(f, LogSeverity_Error, "Unsupported event argument type: %s", typeSignature);
            return;
        }

        SetEventArgumentType(argumentEntity, type);
        SetEventArgumentOffset(argumentEntity, byteOffset);

        byteOffset += GetTypeSize(type);
    }
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
        RegisterProperty(Entity, SubscriptionSender)
        RegisterProperty(Entity, SubscriptionEvent)
    EndComponent()

    __ForceArgumentParse(EventOf_SubscriptionEventChanged());
    __ForceArgumentParse(EventOf_SubscriptionHandlerChanged());
    __ForceArgumentParse(EventOf_SubscriptionSenderChanged());

    RegisterFunction(OnSubscriptionHandlerChanged)\
    RegisterFunction(OnSubscriptionEventChanged)\
    RegisterFunction(OnSubscriptionSenderChanged)\

    RegisterChildCache(EventArgument)

    RegisterSubscription(EventArgumentTypeChanged, OnEventArgumentTypeChanged, 0)
EndUnit()
