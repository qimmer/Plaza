//
// Created by Kim on 28/04/2018.
//

#include <Core/NativeUtils.h>
#include <Core/Event.h>
#include <Core/Vector.h>
#include <Core/Entity.h>
#include <Core/Function.h>
#include <Core/Debug.h>
#include <Core/Property.h>
#include <Core/Algorithms.h>
#include "Identification.h"

#include <stdarg.h>
#include <EASTL/fixed_vector.h>

#define Verbose_Event "event"

struct Subscription {
    Entity SubscriptionHandler, SubscriptionEvent, SubscriptionSender;
};

struct Event {
};

struct EventArgument {
    u32 EventArgumentOffset;
    u8 EventArgumentType;
};

typedef std::vector<Subscription> SubscriptionCacheVector;

static std::vector<SubscriptionCacheVector> EventSubscriptionCache;

static inline SubscriptionCacheVector& GetSubscriptionCache(u32 eventEntityIndex) {
    if(EventSubscriptionCache.size() <= eventEntityIndex) {
        EventSubscriptionCache.resize(eventEntityIndex + 1);
    }

    return EventSubscriptionCache[eventEntityIndex];
}

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

LocalFunction(OnSubscriptionHandlerChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto subscription = GetSubscriptionData (context);

    if(IsEntityValid(subscription->SubscriptionEvent)) {
        AddComponent(subscription->SubscriptionEvent, ComponentOf_Event ());

        auto eventIndex = GetEntityIndex(subscription->SubscriptionEvent);

        auto& cache = GetSubscriptionCache(eventIndex);
        for(auto i = 0; i < cache.size(); ++i) {
            if(cache[i].SubscriptionHandler == oldValue) {
                cache.erase(cache.begin() + i);
                break;
            }
        }

        if(IsEntityValid(newValue)) {
            cache.push_back(*subscription);
        }
    }
}

LocalFunction(OnSubscriptionSenderChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto subscription = GetSubscriptionData (context);

    if(IsEntityValid(subscription->SubscriptionEvent)) {
        AddComponent(subscription->SubscriptionEvent, ComponentOf_Event ());
        auto eventIndex = GetEntityIndex(subscription->SubscriptionEvent);

        auto& cache = GetSubscriptionCache(eventIndex);
        for(auto i = 0; i < cache.size(); ++i) {
            if(cache[i].SubscriptionSender == oldValue) {
                cache[i].SubscriptionSender = newValue;
                break;
            }
        }
    }
}

LocalFunction(OnSubscriptionEventChanged, void, Entity context, Entity oldValue, Entity newValue) {
    auto subscription = GetSubscriptionData (context);

    if(IsEntityValid(oldValue)) {
        AddComponent(oldValue, ComponentOf_Event ());

        auto eventIndex = GetEntityIndex(oldValue);
        auto& cache = GetSubscriptionCache(eventIndex);

        for(auto i = 0; i < cache.size(); ++i) {
            if(cache[i].SubscriptionEvent == subscription->SubscriptionEvent) {
                cache.erase(cache.begin() + i);
                break;
            }
        }
    }

    if(IsEntityValid(newValue) && IsEntityValid(subscription->SubscriptionEvent)) {
        AddComponent(newValue, ComponentOf_Event());

        auto eventIndex = GetEntityIndex(newValue);
        auto& cache = GetSubscriptionCache(eventIndex);

        cache.push_back(*subscription);
    }
}

void __InitializeEvent() {
    auto component = ComponentOf_Event();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Event));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());

    component = ComponentOf_EventArgument();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(EventArgument));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_EventArgumentOffset(), offsetof(EventArgument, EventArgumentOffset), sizeof(EventArgument::EventArgumentOffset), TypeOf_u32,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_EventArgumentType(), offsetof(EventArgument, EventArgumentType), sizeof(EventArgument::EventArgumentType), TypeOf_Type,  component, 0, PropertyKind_Value);

    component = ComponentOf_Subscription();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Subscription));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_SubscriptionEvent(), offsetof(Subscription, SubscriptionEvent), sizeof(Subscription::SubscriptionEvent), TypeOf_Entity,  component, ComponentOf_Event(), PropertyKind_Value);
    __Property(PropertyOf_SubscriptionHandler(), offsetof(Subscription, SubscriptionHandler), sizeof(Subscription::SubscriptionHandler), TypeOf_Entity,  component, ComponentOf_Function(), PropertyKind_Value);
    __Property(PropertyOf_SubscriptionSender(), offsetof(Subscription, SubscriptionSender), sizeof(Subscription::SubscriptionSender), TypeOf_Entity,  component, 0, PropertyKind_Value);
}

API_EXPORT Variant FireEventFast(Entity event, u32 numArguments, const Variant* arguments) {

    Verbose(Verbose_Event, "Firing event %llu ...", event);

    auto data = GetEventData(event);

    auto eventIndex = GetEntityIndex(event);
    auto& cache = GetSubscriptionCache(eventIndex);

    Variant returnData;
    memset(&returnData, 0, sizeof(Variant));

    if(data) {
        auto sender = arguments[0].as_Entity;
        for(auto i = 0; i < cache.size(); ++i) {
            auto& subscription = cache[i];
            if(!subscription.SubscriptionSender || !sender || subscription.SubscriptionSender == sender) {
                if(IsEntityValid(subscription.SubscriptionHandler)) {
                    returnData = CallFunction(subscription.SubscriptionHandler, numArguments, arguments);
                }
            }
        }
    }

    return returnData;
}

API_EXPORT Entity GetSubscriptionHandler(Entity entity) {
    auto data = GetSubscriptionData(entity);
    if(data) return data->SubscriptionHandler;
    return 0;
}

API_EXPORT Entity GetSubscriptionSender(Entity entity) {
    auto data = GetSubscriptionData(entity);
    if(data) return data->SubscriptionSender;
    return 0;
}

API_EXPORT Entity GetSubscriptionEvent(Entity entity) {
    auto data = GetSubscriptionData(entity);
    if(data) return data->SubscriptionEvent;
    return 0;
}

API_EXPORT void SetSubscriptionHandler(Entity entity, Entity value)  {
    static Entity prop = PropertyOf_SubscriptionHandler ();
    auto oldValue = GetSubscriptionHandler(entity);
    SetPropertyValue(prop, entity, MakeVariant(Entity, value));

    OnSubscriptionHandlerChanged(entity, oldValue, value);
}

API_EXPORT void SetSubscriptionEvent(Entity entity, Entity value)  {
    static Entity prop = PropertyOf_SubscriptionEvent ();
    auto oldValue = GetSubscriptionEvent(entity);
    SetPropertyValue(prop, entity, MakeVariant(Entity, value));

    OnSubscriptionEventChanged(entity, oldValue, value);
}

API_EXPORT void SetSubscriptionSender(Entity entity, Entity value)  {
    static Entity prop = PropertyOf_SubscriptionSender ();
    auto oldValue = GetSubscriptionSender(entity);
    SetPropertyValue(prop, entity, MakeVariant(Entity, value));

    OnSubscriptionSenderChanged(entity, oldValue, value);
}

API_EXPORT void SetEventArgsByDecl(Entity event, StringRef arguments) {
    auto len = strlen(arguments);
    auto offset = 0;
    auto byteOffset = 0;

    char argumentSplits[PathMax];
    strcpy(argumentSplits, arguments);

    while(offset < len) {
        auto argument = &argumentSplits[0] + offset;
        auto nextComma = strchr(&argumentSplits[0] + offset, ',');
        if(!nextComma) nextComma = &argumentSplits[0] + len;
        auto argumentLength = nextComma - argument;
        offset += argumentLength + 1;
        *nextComma = '\0';

        auto name = strrchr(argument, ' ');
        Assert(event, name);
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

        auto argumentEntity = AddEventArguments(event);

        SetName(argumentEntity, name);

        auto type = 0;
        for(auto i = 0; i < TypeOf_MAX; ++i) {
            if(strcmp(typeSignature, GetTypeName(i)) == 0) {
                type = i;
                break;
            }
        }

        if(!type) {
            Log(event, LogSeverity_Error, "Unsupported event argument type: %s", typeSignature);
            return;
        }

        SetEventArgumentType(argumentEntity, type);
        SetEventArgumentOffset(argumentEntity, byteOffset);

        byteOffset += GetTypeSize(type);
    }
}

__PropertyCoreImpl(u32, EventArgumentOffset, EventArgument)
__PropertyCoreImpl(Type, EventArgumentType, EventArgument)
__ArrayPropertyCoreImpl(EventArgument, EventArguments, Event)

BeginUnit(Event)
    BeginComponent(EventArgument)
        RegisterProperty(u32, EventArgumentOffset)
        RegisterProperty(Type, EventArgumentType)
    EndComponent()

    BeginComponent(Event)
        RegisterArrayProperty(EventArgument, EventArguments)
    EndComponent()

    BeginComponent(Subscription)
        RegisterProperty(Entity, SubscriptionHandler)
        RegisterProperty(Entity, SubscriptionSender)
        RegisterProperty(Entity, SubscriptionEvent)
    EndComponent()

    RegisterFunction(OnSubscriptionHandlerChanged)
    RegisterFunction(OnSubscriptionEventChanged)
    RegisterFunction(OnSubscriptionSenderChanged)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_EventArgumentType()), OnEventArgumentTypeChanged, 0)
EndUnit()
