//
// Created by Kim on 07/06/2018.
//

#ifndef PLAZA_NATIVEUTILS_H
#define PLAZA_NATIVEUTILS_H

#include "Types.h"
#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

typedef u64 Entity;

typedef bool(*FunctionCaller)(u64 functionImplementation, Type returnArgumentTypeIndex, void *returnData, u32 numArguments, const Type *argumentTypes, const void **argumentDataPtrs);

Entity ModuleOf_Core();

Entity CreateEntityFromName(Entity parent, StringRef name);

void SetModuleSourcePath(Entity module, StringRef sourcePath);
void SetModuleVersion(Entity module, StringRef version);
Entity GetModuleRoot();
Entity GetUnregisteredEntitiesRoot();
void SetName(Entity entity, StringRef name);
bool AddComponent(Entity entity, Entity type);
bool HasComponent(Entity entity, Entity component);
u32 GetNumComponents(Entity component);
Entity GetComponentEntity(Entity component, u32 index);
char * GetComponentData(Entity component, u32 index);
u32 GetComponentIndex(Entity entity, Entity component);
Entity GetComponentAddedEvent(Entity component);
Entity GetComponentRemovedEvent(Entity component);
void SetComponentSize(Entity entity, u16 size);
void SetEventArgsByDecl(Entity entity, StringRef decl);
void SetEventArgumentOffset(Entity entity, u32 offset);
void SetEventArgumentType(Entity entity, Type type);
void SetPropertyType(Entity entity, Type type);
void SetPropertyValue(Entity entity, Entity context, const void *valueData);
void SetPropertyOffset(Entity entity, u32 offset);
void SetPropertySize(Entity entity, u32 size);
void SetPropertyChangedEvent(Entity entity, Entity event);
void SetPropertyComponent(Entity entity, Entity component);
bool GetPropertyValue(Entity entity, Entity context, void *dataOut);
void SetParent(Entity entity, Entity parent);
Entity GetParent(Entity entity);
void FireEvent(Entity event, Entity context, ...);
void FireEventFast(
        Entity event,
        u32 numArguments,
        const Type *argumentTypes,
        const void **argumentDataPtrs
);
void SetEventSize(Entity event, u32 size);

void SetFunctionCaller(Entity entity, FunctionCaller caller);
void SetFunctionImplementation(Entity entity, u64 impl);
void SetFunctionReturnType(Entity entity, Entity returnType);
void SetFunctionArgsByDecl(Entity entity, StringRef decl);

bool CallNativeFunction(
        u64 funcPtr,
        Type returnArgumentTypeIndex,
        void *returnData,
        u32 numArguments,
        const Type *argumentTypes,
        const void **argumentDataPtrs
);

Entity EventOf_ParentChanged();

Entity GetUniqueEntity(StringRef name, bool *firstTime);

template <class... T>
inline void __FireEventVa(Entity event, T... args) {
    FireEvent(event, args...);
}

// Declaration Macros

#define Declare(TYPE, NAME, INITIALIZER) \
    inline Entity TYPE ## Of_ ## NAME () {\
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(entity == 0) {\
            entity = GetUniqueEntity(#TYPE " " #NAME, &firstTime);\
            if(firstTime) {\
                SetName(entity, #TYPE " " #NAME);\
                SetParent(entity, GetUnregisteredEntitiesRoot());\
            }\
        }\
        return entity;\
    }

#define __Declare(TYPE, NAME, INITIALIZER) \
    inline Entity TYPE ## Of_ ## NAME () {\
        static Entity entity = 0;\
        if(entity == 0) {\
            entity = GetUniqueEntity(#TYPE " " #NAME, NULL);\
        }\
        return entity;\
    }
#define Module(NAME) \
    Entity ModuleOf_ ## NAME ();

#define Unit(NAME) \
    void __InitUnit_ ## NAME (Entity module);

#define Node(NAME) \
    Declare(Node, NAME, 0)

#define Component(NAME) \
    struct NAME;\
    Declare(Component, NAME, 0)\
    inline NAME * Get ## NAME ## Data(Entity entity) {\
        return (NAME*)GetComponentData(ComponentOf_ ## NAME(), GetComponentIndex(ComponentOf_ ## NAME(), entity));\
    }

#define __Component(NAME) \
    struct NAME;\
    __Declare(Component, NAME, 0)\
    inline NAME * Get ## NAME ## Data(Entity entity) {\
        return (NAME*)GetComponentData(ComponentOf_ ## NAME(), GetComponentIndex(ComponentOf_ ## NAME(), entity));\
    }

#define Event(NAME, ...) \
    static const StringRef __Event ## NAME = #__VA_ARGS__;\
    Declare(Event, NAME, 0)

#define __Event(NAME, ...) \
    static const StringRef __Event ## NAME = #__VA_ARGS__;\
    __Declare(Event, NAME, 0)

#define __PropertyCore(PROPERTYTYPE, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME, 0) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    Event(PROPERTYNAME ## Changed, Entity changedEntity, PROPERTYTYPE oldValue, PROPERTYTYPE newValue)\
    inline PROPERTYTYPE Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        PROPERTYTYPE value;\
        memset(&value, 0, sizeof(PROPERTYTYPE));\
        GetPropertyValue(prop, entity, &value);\
        return value;\
    }\
    void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE value);

#define Property(PROPERTYTYPE, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME, 0) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    Event(PROPERTYNAME ## Changed, Entity changedEntity, PROPERTYTYPE oldValue, PROPERTYTYPE newValue)\
    inline PROPERTYTYPE Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        PROPERTYTYPE value;\
        memset(&value, 0, sizeof(PROPERTYTYPE));\
        GetPropertyValue(prop, entity, &value);\
        return value;\
    }\
    inline void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE value) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME ();\
        SetPropertyValue(prop, entity, &value);\
    }

#define __PropertyNoInit(PROPERTYTYPE, PROPERTYNAME, ...) \
    __Declare(Property, PROPERTYNAME, 0) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    Event(PROPERTYNAME ## Changed, Entity changedEntity, PROPERTYTYPE oldValue, PROPERTYTYPE newValue)\
    inline PROPERTYTYPE Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        PROPERTYTYPE value;\
        memset(&value, 0, sizeof(PROPERTYTYPE));\
        GetPropertyValue(prop, entity, &value);\
        return value;\
    }\
    inline void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE value) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME ();\
        SetPropertyValue(prop, entity, &value);\
    }

#define __Function(NAME, R, ...) \
    inline Entity FunctionOf_ ## NAME () {\
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(!entity) {\
            entity = GetUniqueEntity("Function " #NAME " " __FILE__, &firstTime);\
            if(firstTime) {\
                char unregisteredName[64];\
                snprintf(unregisteredName, 64, "Func_" #NAME "_%llu", entity);\
                SetParent(entity, GetUnregisteredEntitiesRoot());\
                SetName(entity, unregisteredName);\
                SetFunctionReturnType(entity, TypeOf_ ## R);\
                SetFunctionCaller(entity, CallNativeFunction);\
                SetFunctionImplementation(entity, (u64)(void*) (Entity(*)(StringRef)) & NAME );\
                SetFunctionArgsByDecl(entity, #__VA_ARGS__);\
            }\
        }\
        return entity;\
    }

#define Function(NAME, R, ...) \
    R NAME( __VA_ARGS__ ); \
    __Function(NAME, R, ##__VA_ARGS__)

#define Test(NAME) \
    TestResult NAME(Entity test);\
    inline Entity FunctionOf_ ## NAME () {\
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(!entity) {\
            entity = GetUniqueEntity("Test " #NAME " " __FILE__, &firstTime);\
            if(firstTime) {\
                char unregisteredName[64];\
                snprintf(unregisteredName, 64, "Test_%llu", entity);\
                SetParent(entity, GetUnregisteredEntitiesRoot());\
                SetName(entity, unregisteredName);\
                SetFunctionReturnType(entity, TypeOf_u32);\
                SetFunctionImplementation(entity, (u64)(void*)NAME);\
                SetFunctionCaller(entity, CallNativeFunction);\
                SetFunctionArgsByDecl(entity, "");\
                AddComponent(entity, ComponentOf_Test());\
            }\
        }\
        return entity;\
    }\
    TestResult NAME(Entity test)

#define LocalFunction(NAME, R, ...) \
    static R NAME(__VA_ARGS__);\
    __Function(NAME, R, __VA_ARGS__) \
    static R NAME(__VA_ARGS__)


#define ReferenceTracker(TRACKEDPROPERTY, REFERENCEPROPERTY, REFERENCECOMPONENT, VECTORNAME) \
    LocalFunction(On ## TRACKEDPROPERTY ## Changed, void, Entity context, Entity oldValue, Entity newValue) { \
        auto reference = Get ## REFERENCEPROPERTY (context); \
     \
        if(IsEntityValid(reference)) { \
            AddComponent(reference, ComponentOf_ ## REFERENCECOMPONENT ());\
            auto data = Get ## REFERENCECOMPONENT (reference); \
     \
            for(auto i = 0; i < data->Num ## VECTORNAME; ++i) { \
                if(data-> VECTORNAME [i] == oldValue) { \
                    VectorRemove(data, VECTORNAME, i); \
                    break; \
                } \
            } \
     \
            if(IsEntityValid(newValue)) { \
                VectorAdd(data, VECTORNAME, newValue); \
            } \
        } \
    } \
     \
    LocalFunction(On ## REFERENCEPROPERTY ## Changed, void, Entity context, Entity oldValue, Entity newValue) { \
        auto tracked = Get ## TRACKEDPROPERTY(context); \
     \
        if(IsEntityValid(oldValue)) { \
            AddComponent(oldValue, ComponentOf_ ## REFERENCECOMPONENT ());\
            auto data = Get ## REFERENCECOMPONENT (oldValue); \
     \
            for(auto i = 0; i < data->Num ## VECTORNAME; ++i) { \
                if(data-> VECTORNAME [i] == tracked) { \
                    VectorRemove(data, VECTORNAME, i); \
                    break; \
                } \
            } \
        } \
     \
        if(IsEntityValid(newValue) && IsEntityValid(tracked)) { \
            auto data = GetEvent(newValue); \
            VectorAdd(data, VECTORNAME, newValue); \
        } \
    }

#define ChildCache(PARENTCOMPONENT, CHILDCOMPONENT, VECTORNAME) \
    LocalFunction(On ## CHILDCOMPONENT ## ParentChanged, void, Entity context, Entity oldParent, Entity newParent) { \
        static auto parentComponent = ComponentOf_ ## PARENTCOMPONENT ();\
        static auto childComponent = ComponentOf_ ## CHILDCOMPONENT ();\
        if(HasComponent(context, childComponent)) { \
            auto childData = Get ## CHILDCOMPONENT ## Data(context);\
            if(HasComponent(oldParent, parentComponent)) {\
                auto data = Get ## PARENTCOMPONENT ## Data(oldParent);\
                for(auto i = 0; i < data->Num ## VECTORNAME; ++i) { \
                    if(data-> VECTORNAME [i] == childData) { \
                        VectorRemove(data, VECTORNAME, i); \
                        break; \
                    } \
                } \
            } \
            if(HasComponent(newParent, parentComponent)) {\
                auto data = Get ## PARENTCOMPONENT ## Data(newParent);\
                VectorAdd(data, VECTORNAME, childData); \
            } \
        } \
    } \

// Definition Macros

// |
// |-+ Module
//   |
// === Unit 1 ===
//   |
//   |-+ Component 1
//     |-- Property 1
//     |-- Property 2
//   |
//   |-+ Event 1
//     |-- Argument 1
//     |-- Argument 2
//   |
//   |-- Function 1
//   |
// === Unit 2 ===
//   |
//   |-- Subscription 1

#define BeginModule(NAME) \
    void __InitModule_ ## NAME(Entity module);\
    API_EXPORT Entity ModuleOf_ ## NAME () {\
        static Entity module = 0;\
        static bool firstTime = false;\
        if(!module) {\
            ModuleOf_Core();\
            module = GetUniqueEntity("Module " #NAME, &firstTime);\
            if(firstTime) {\
                SetName(module, #NAME);\
                SetParent(module, GetModuleRoot());\
                SetModuleSourcePath(module, __FILE__);\
                SetModuleVersion(module, __DATE__ " " __TIME__);\
                __InitModule_ ## NAME(module);\
            }\
        }\
        return module;\
    }\
    void __InitModule_ ## NAME(Entity module) {


#define RegisterUnit(NAME) \
        __InitUnit_ ## NAME (module);

#define RegisterDependency(MODULE) \
        ModuleOf_ ## MODULE (); // Reference and call module initializer

#define EndModule() \
        FireEvent(EventOf_ModuleInitialized(), module);\
    }

#define BeginUnit(NAME) \
    API_EXPORT void __InitUnit_ ## NAME (Entity module) {\
        static bool initialized = false;\
        if(initialized) return;\
        initialized = true;\
        Entity node = 0, type = 0, component = 0, property = 0, event = 0, argument = 0, argumentParent = 0, base = 0, extension = 0, function = 0, subscription = 0, subscriptionFunction = 0;\
        auto unitName = #NAME;\
        char buffer[1024];

#define BeginComponent(COMPONENT) \
        component = ComponentOf_ ## COMPONENT ();\
        SetParent(component, module);\
        SetName(component, #COMPONENT);\
        AddComponent(component, ComponentOf_Component());\
        SetComponentSize(component, sizeof(COMPONENT));\
        {\
            typedef COMPONENT ComponentType;

#define EndComponent() \
        }

#define RegisterNode(NAME) \
        node = NodeOf_ ## NAME ();\
        SetName(node, #NAME);\
        SetParent(node, module);

#define RegisterEvent(NAME) \
        event = EventOf_ ## NAME ();\
        SetName(event, #NAME);\
        SetParent(event, module);\
        AddComponent(event, ComponentOf_Event());\
        SetEventArgsByDecl(event, __Event ## NAME);

#define RegisterProperty(PROPERTYTYPE, PROPERTYNAME)\
    RegisterEvent(PROPERTYNAME ## Changed)\
    property = PropertyOf_ ## PROPERTYNAME ();\
    SetParent(property, component);\
    SetName(property, #PROPERTYNAME);\
    SetPropertyType(property, TypeOf_ ## PROPERTYTYPE);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyChangedEvent(property, EventOf_ ## PROPERTYNAME ## Changed ());\
    SetPropertyComponent(property, component);\
    SetPropertyMeta(property, __ ## PROPERTYNAME ## __Meta);

#define RegisterBase(BASECOMPONENT) \
    base = CreateEntityFromName(component, #BASECOMPONENT);\
    SetName(base, #BASECOMPONENT);\
    SetBaseComponent(base, component);

#define RegisterExtension(EXTENSIONCOMPONENT) \
    extension = CreateEntityFromName(component, #EXTENSIONCOMPONENT);\
    SetName(extension, #EXTENSIONCOMPONENT);\
    SetExtensionComponent(extension, component);

#define RegisterFunction(FUNCTION) \
    function = FunctionOf_ ## FUNCTION ();\
    SetParent(function, module);\
    SetName(function, #FUNCTION);

#define RegisterTest(TEST) \
    RegisterFunction(TEST)\
    AddComponent(function, ComponentOf_Test());

#define RegisterSubscription(EVENT, FUNCTION, SENDER) \
    snprintf(buffer, 1024, "Subscription_%s_%s", unitName, #EVENT);\
    subscription = CreateEntityFromName(module, buffer);\
    subscriptionFunction = FunctionOf_ ## FUNCTION ();\
    SetSubscriptionEvent(subscription, EventOf_ ## EVENT ());\
    SetSubscriptionHandler(subscription, subscriptionFunction);\
    SetSubscriptionSender(subscription, SENDER);\
    if(!IsEntityValid(GetParent(subscriptionFunction))) {\
        SetParent(subscriptionFunction, module);\
    }

#define RegisterReferenceTracker(TRACKEDPROPERTY, REFERENCEPROPERTY) \
    RegisterFunction(On ## TRACKEDPROPERTY ## Changed)\
    RegisterFunction(On ## REFERENCEPROPERTY ## Changed)\
    RegisterSubscription(TRACKEDPROPERTY ## Changed, On ## TRACKEDPROPERTY ## Changed) \
    RegisterSubscription(REFERENCEPROPERTY ## Changed, On ## REFERENCEPROPERTY ## Changed)

#define RegisterChildCache(CHILDCOMPONENT) \
    RegisterFunction(On ## CHILDCOMPONENT ## ParentChanged)\
    RegisterSubscription(ParentChanged, On ## CHILDCOMPONENT ## ParentChanged, 0)

#define RegisterTimer(FUNCTION, INTERVAL) \
    auto timer_ ## FUNCTION = CreateEntityFromName(module, #FUNCTION "Timer");\
    SetTimerInterval(timer_ ## FUNCTION, (INTERVAL));\
    SetTimerRepeat(timer_ ## FUNCTION, true);\
    RegisterSubscription( TimerTick, FUNCTION, timer_ ## FUNCTION )

#define EndUnit() \
    }

#include <Core/Module.h>
#include <Core/Property.h>
#include <Core/Entity.h>
#include <Core/Component.h>
#include <Core/Event.h>
#include <Core/Node.h>

#endif //PLAZA_NATIVEUTILS_H
