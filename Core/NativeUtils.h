//
// Created by Kim on 07/06/2018.
//

#ifndef PLAZA_NATIVEUTILS_H
#define PLAZA_NATIVEUTILS_H

#include "Types.h"
#include <stddef.h>
#include <memory.h>

typedef u64 Entity;

typedef bool(*FunctionCaller)(u64 functionImplementation, Type returnArgumentTypeIndex, void *returnData, u32 numArguments, const Type *argumentTypes, const void **argumentDataPtrs);

Entity CreateEntityFromName(Entity parent, StringRef name);

void SetModuleSourcePath(Entity module, StringRef sourcePath);
void SetModuleVersion(Entity module, StringRef version);
Entity GetModuleRoot();
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
void SetEventArgumentSize(Entity entity, u32 size);
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
void FireEventNative(Entity event, const void *eventArgsStruct);
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

Entity GetUniqueEntity(StringRef name);

// Declaration Macros

#define Declare(TYPE, NAME, INITIALIZER) \
    inline Entity TYPE ## Of_ ## NAME () {\
        static Entity entity = GetUniqueEntity(#TYPE " " #NAME);\
        return entity;\
    }

#define Module(NAME) \
    Entity ModuleOf_ ## NAME ();

#define Unit(NAME) \
    void __InitUnit_ ## NAME (Entity module);

#define Component(NAME) \
    struct NAME;\
    Declare(Component, NAME, 0)\
    inline NAME * Get ## NAME (Entity entity) {\
        return (NAME*)GetComponentData(ComponentOf_ ## NAME(), GetComponentIndex(ComponentOf_ ## NAME(), entity));\
    }\
    inline Entity EventOf_ ## NAME ## Added () {\
        return GetComponentAddedEvent(ComponentOf_ ## NAME());\
    } \
    inline Entity EventOf_ ## NAME ## Removed () {\
        return GetComponentRemovedEvent(ComponentOf_ ## NAME());\
    }

#define Event(EVENT) \
    Declare(Event, EVENT, 0)\
    inline void Fire ## EVENT (EVENT ## Args arguments) { \
        FireEventNative(EventOf_ ## EVENT (), &arguments);\
    }

#define __PropertyCore(PROPERTYTYPE, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME, 0) \
    struct PROPERTYNAME ## ChangedArgs { Entity ChangedEntity; PROPERTYTYPE OldValue; PROPERTYTYPE NewValue; };\
    Event(PROPERTYNAME ## Changed)\
    inline PROPERTYTYPE Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        PROPERTYTYPE value;\
        memset(&value, 0, sizeof(PROPERTYTYPE));\
        GetPropertyValue(prop, entity, &value);\
        return value;\
    }\
    void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE value);

#define Property(PROPERTYTYPE, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME, 0) \
    struct PROPERTYNAME ## ChangedArgs { Entity ChangedEntity; PROPERTYTYPE OldValue; PROPERTYTYPE NewValue; };\
    Event(PROPERTYNAME ## Changed)\
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

#define Function(NAME, R, ...) \
    R NAME(__VA_ARGS__); \
    inline Entity FunctionOf_ ## NAME () {\
        static Entity entity = 0;\
        if(!entity) {\
            entity = GetUniqueEntity("Function " #NAME);\
            SetName(entity, #NAME);\
            SetFunctionCaller(entity, CallNativeFunction);\
            SetFunctionImplementation(entity, (u64)(void*)NAME);\
            SetFunctionReturnType(entity, TypeOf_ ## R);\
            SetFunctionArgsByDecl(entity, #__VA_ARGS__);\
        }\
        return entity;\
    }

#define Test(NAME) \
    TestResult NAME();\
    inline Entity FunctionOf_ ## NAME () {\
        static Entity entity = 0;\
        if(!entity) {\
            entity = GetUniqueEntity("Function " #NAME);\
            SetName(entity, #NAME);\
            SetFunctionCaller(entity, CallNativeFunction);\
            SetFunctionImplementation(entity, (u64)(void*)NAME);\
            SetFunctionReturnType(entity, TypeOf_u32);\
            SetFunctionArgsByDecl(entity, "");\
            AddComponent(entity, ComponentOf_Test());\
        }\
        return entity;\
    }\
    TestResult NAME()

#define LocalFunction(NAME, R, ...) \
    Function(NAME, R, __VA_ARGS__) \
    R NAME(__VA_ARGS__)


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
            auto childData = Get ## CHILDCOMPONENT (context);\
            if(HasComponent(oldParent, parentComponent)) {\
                auto data = Get ## PARENTCOMPONENT (oldParent);\
                for(auto i = 0; i < data->Num ## VECTORNAME; ++i) { \
                    if(data-> VECTORNAME [i] == childData) { \
                        VectorRemove(data, VECTORNAME, i); \
                        break; \
                    } \
                } \
            } \
            if(HasComponent(newParent, parentComponent)) {\
                auto data = Get ## PARENTCOMPONENT (newParent);\
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
        if(!module) {\
            module = GetUniqueEntity("Module " #NAME);\
            __InitModule_ ## NAME(module);\
            SetModuleSourcePath(module, __FILE__);\
            SetModuleVersion(module, __DATE__ " " __TIME__);\
            SetName(module, #NAME);\
            SetParent(module, GetModuleRoot());\
        }\
        return module;\
    }\
    void __InitModule_ ## NAME(Entity module) {


#define RegisterUnit(NAME) \
        __InitUnit_ ## NAME (module);

#define RegisterDependency(MODULE) \
        ModuleOf_ ## MODULE (); // Reference and call module initializer

#define EndModule() \
    }

#define BeginUnit(NAME) \
    API_EXPORT void __InitUnit_ ## NAME (Entity module) {\
        static bool initialized = false;\
        if(initialized) return;\
        initialized = true;\
        Entity type = 0, component = 0, property = 0, event = 0, argument = 0, argumentParent = 0, base = 0, extension = 0, function = 0, subscription = 0, subscriptionFunction = 0;\
        auto unitName = #NAME;\
        char buffer[1024];

#define BeginComponent(COMPONENT) \
        component = ComponentOf_ ## COMPONENT ();\
        SetParent(component, module);\
        SetName(component, #COMPONENT);\
        SetComponentSize(component, sizeof(COMPONENT));\
        {\
            typedef COMPONENT ComponentType;

#define EndComponent() \
        }

#define BeginEvent(NAME) \
        event = EventOf_ ## NAME ();\
        SetName(event, #NAME);\
        SetParent(event, module);\
        AddComponent(event, ComponentOf_Event());\
        { \
            typedef NAME ## Args EventArgs;

#define EndEvent() \
        }

#define EventArgument(TYPE, NAME) \
        argument = CreateEntityFromName(event, #NAME);\
        SetName(argument, #NAME);\
        SetParent(argument, event);\
        SetEventArgumentType(argument, TypeOf_ ## TYPE);\
        SetEventArgumentOffset(argument, offsetof(EventArgs, NAME));

#define RegisterProperty(PROPERTYTYPE, PROPERTYNAME)\
    BeginEvent(PROPERTYNAME ## Changed)\
        EventArgument(PROPERTYTYPE, OldValue)\
        EventArgument(PROPERTYTYPE, NewValue)\
    EndEvent()\
    property = PropertyOf_ ## PROPERTYNAME ();\
    SetName(property, #PROPERTYNAME);\
    SetPropertyType(property, TypeOf_ ## PROPERTYTYPE);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyChangedEvent(property, EventOf_ ## PROPERTYNAME ## Changed ());\
    SetPropertyComponent(property, component);\
    SetParent(property, component);

#define RegisterBase(BASECOMPONENT) \
    base = CreateEntityFromName(component, #BASECOMPONENT);\
    SetName(base, #BASECOMPONENT);\
    SetDependencyComponent(base, component);

#define RegisterExtension(EXTENSIONCOMPONENT) \
    extension = CreateEntityFromName(component, #EXTENSIONCOMPONENT);\
    SetName(extension, #EXTENSIONCOMPONENT);\
    SetExtensionComponent(extension, component);

#define RegisterFunction(FUNCTION) \
    function = FunctionOf_ ## FUNCTION ();\
    SetParent(function, module);

#define RegisterTest(TEST) \
    RegisterFunction(TEST)

#define RegisterSubscription(EVENT, FUNCTION) \
    snprintf(buffer, 1024, "Subscription_%s_%s", unitName, #EVENT);\
    subscription = CreateEntityFromName(module, buffer);\
    subscriptionFunction = FunctionOf_ ## FUNCTION ();\
    SetSubscriptionEvent(subscription, EventOf_ ## EVENT ());\
    SetSubscriptionHandler(subscription, subscriptionFunction);\
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
    RegisterSubscription(ParentChanged, On ## CHILDCOMPONENT ## ParentChanged)

#define EndUnit() \
    }

#include <Core/Module.h>
#include <Core/Property.h>
#include <Core/Entity.h>
#include <Core/Component.h>
#include <Core/Event.h>
#include <Core/Hierarchy.h>

#endif //PLAZA_NATIVEUTILS_H
