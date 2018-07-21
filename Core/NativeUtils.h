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
#include <Core/Vector.h>

typedef u64 Entity;

typedef bool(*FunctionCaller)(u64 functionImplementation, Type returnArgumentTypeIndex, void *returnData, u32 numArguments, const Type *argumentTypes, const void **argumentDataPtrs);

#define PropertyKind_Value 1
#define PropertyKind_String 2
#define PropertyKind_Child 3
#define PropertyKind_Array 4

Entity ModuleOf_Core();

u32 GetArrayPropertyCount(Entity property, Entity entity);
u32 AddArrayPropertyElement(Entity property, Entity entity);
bool RemoveArrayPropertyElement(Entity property, Entity entity, u32 index);
Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index);
Entity *GetArrayPropertyElements(Entity property, Entity entity);

void SetModuleSourcePath(Entity module, StringRef sourcePath);
void SetModuleVersion(Entity module, StringRef version);
Entity GetModuleRoot();
Entity GetUnregisteredEntitiesRoot();
void SetName(Entity entity, StringRef name);
void SetOwner(Entity entity, Entity owner, Entity ownerProperty);
bool AddComponent(Entity entity, Entity type);
bool HasComponent(Entity entity, Entity component);
u32 GetComponentMax(Entity component);
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
void SetPropertyChildComponent(Entity entity, Entity component);
void SetPropertyKind(Entity property, u8 kind);
bool GetPropertyValue(Entity entity, Entity context, void *dataOut);
Entity GetOwner(Entity entity);
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

#define Declare(TYPE, NAME) \
    inline Entity TYPE ## Of_ ## NAME () {\
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(entity == 0) {\
            entity = GetUniqueEntity(#TYPE " " #NAME, &firstTime);\
        }\
        return entity;\
    }

#define Component(NAME) \
    struct NAME;\
    Declare(Component, NAME)\
    inline NAME * Get ## NAME ## Data(Entity entity) {\
        return (NAME*)GetComponentData(ComponentOf_ ## NAME(), GetComponentIndex(ComponentOf_ ## NAME(), entity));\
    }

#define Module(NAME) \
    Entity ModuleOf_ ## NAME ();

#define Unit(NAME) \
    void __InitUnit_ ## NAME (Entity module);

#define Event(NAME, ...) \
    static const StringRef __Event ## NAME = #__VA_ARGS__;\
    Declare(Event, NAME)

#define __PropertyCoreGetOnly(COMPONENT, PROPERTYTYPE, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    Event(PROPERTYNAME ## Changed, Entity changedEntity, PROPERTYTYPE oldValue, PROPERTYTYPE newValue)\
    PROPERTYTYPE Get ## PROPERTYNAME(Entity entity);\

#define __PropertyCore(COMPONENT, PROPERTYTYPE, PROPERTYNAME, ...) \
    __PropertyCoreGetOnly(COMPONENT, PROPERTYTYPE, PROPERTYNAME, ##__VA_ARGS__ )\
    void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE value);


#define __PropertyCoreGet(TYPE, PROPERTY, COMPONENT) \
    API_EXPORT TYPE Get ## PROPERTY(Entity entity) {\
        auto data = Get ## COMPONENT ## Data(entity);\
        if(data) return data-> PROPERTY;\
        return 0;\
    }

#define __PropertyCoreSet(TYPE, PROPERTY, COMPONENT) \
    API_EXPORT void Set ## PROPERTY (Entity entity, TYPE value) {\
        AddComponent(entity, ComponentOf_ ## COMPONENT ());\
        Get ## COMPONENT ## Data(entity)->PROPERTY = value;\
    }

#define __PropertyCoreImpl(TYPE, PROPERTY, COMPONENT) \
    __PropertyCoreGet(TYPE, PROPERTY, COMPONENT)\
    __PropertyCoreSet(TYPE, PROPERTY, COMPONENT)


#define Property(PROPERTYTYPE, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
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

#define ChildProperty(CHILDCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    inline Entity Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        Entity value = 0;\
        GetPropertyValue(prop, entity, &value);\
        return value;\
    }


#define __ChildPropertyCore(CHILDCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    Entity Get ## PROPERTYNAME(Entity entity);


#define __ChildPropertyCoreImpl(TYPE, PROPERTY, COMPONENT) \
    API_EXPORT TYPE Get ## PROPERTY(Entity entity) {\
        auto data = Get ## COMPONENT ## Data(entity);\
        if(data) return data-> PROPERTY;\
        return 0;\
    }


#define ArrayProperty(ELEMENTCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    inline const Entity* Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        Entity *value = 0;\
        GetPropertyValue(prop, entity, &value);\
        return value;\
    }\
    inline u32 GetNum ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        return GetArrayPropertyCount(prop, entity);\
    }\
    inline Entity Add ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        auto index = AddArrayPropertyElement(prop, entity);\
        return GetArrayPropertyElement(prop, entity, index);\
    }\
    inline void Remove ## PROPERTYNAME(Entity entity, u32 index) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        RemoveArrayPropertyElement(prop, entity, index);\
    }


#define __ArrayPropertyCore(ELEMENTCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = #__VA_ARGS__;\
    const Entity* Get ## PROPERTYNAME(Entity entity);\
    u32 GetNum ## PROPERTYNAME(Entity entity);\
    Entity Add ## PROPERTYNAME(Entity entity);\
    inline void Remove ## PROPERTYNAME(Entity entity, u32 index) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        RemoveArrayPropertyElement(prop, entity, index);\
    }

#define __ArrayPropertyCoreImpl(ELEMENTCOMPONENT, PROPERTYNAME, COMPONENT) \
    API_EXPORT const Entity* Get ## PROPERTYNAME(Entity entity) {\
        auto data = Get ## COMPONENT ## Data(entity);\
        if(data) return GetVector(data-> PROPERTYNAME);\
        return 0;\
    }\
    API_EXPORT u32 GetNum ## PROPERTYNAME(Entity entity) {\
        auto data = Get ## COMPONENT ## Data(entity);\
        if(data) return data-> PROPERTYNAME .Count;\
        return 0;\
    }\
    API_EXPORT Entity Add ## PROPERTYNAME(Entity entity) {\
        auto element = CreateEntity();\
        auto valueData = Get ## COMPONENT ## Data(entity);\
        VectorAdd(valueData->PROPERTYNAME, element);\
        AddComponent(element, ComponentOf_Ownership());\
        SetOwner(element, entity, PropertyOf_ ## PROPERTYNAME ());\
        AddComponent(element, ComponentOf_ ## ELEMENTCOMPONENT ());\
        return element;\
    }

#define __Function(FILE, NAME, R, ...) \
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(!entity) {\
            entity = GetUniqueEntity("Function " #NAME " " FILE, &firstTime);\
            if(firstTime) {\
                char unregisteredName[64];\
                snprintf(unregisteredName, 64, "Func_" #NAME "_%llu", entity);\
                SetName(entity, unregisteredName);\
                SetFunctionReturnType(entity, TypeOf_ ## R);\
                SetFunctionCaller(entity, CallNativeFunction);\
                SetFunctionImplementation(entity, (u64)(void*) (Entity(*)(StringRef)) & NAME );\
                SetFunctionArgsByDecl(entity, #__VA_ARGS__);\
            }\
        }\
        return entity;

#define Function(NAME, R, ...) \
    R NAME( __VA_ARGS__ ); \
    inline Entity FunctionOf_ ## NAME () {\
        __Function(__FILE__, NAME, R, ##__VA_ARGS__)\
    }

#define Test(NAME) \
    Function(NAME, s32, Entity test)\
    s32 NAME (Entity test)

#define LocalFunction(NAME, R, ...) \
    static R NAME(__VA_ARGS__);\
    static Entity FunctionOf_ ## NAME () {\
        __Function(__FILE__, NAME, R, __VA_ARGS__) \
    }\
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

#define BeginModule(NAME, ...) \
    void __InitModule_ ## NAME(Entity module);\
    API_EXPORT Entity ModuleOf_ ## NAME () {\
        static Entity module = 0;\
        static bool firstTime = false;\
        if(!module) {\
            ModuleOf_Core();\
            module = GetUniqueEntity("Module " #NAME, &firstTime);\
            if(firstTime) {\
                SetName(module, #NAME);\
                SetModuleSourcePath(module, __FILE__);\
                SetModuleVersion(module, __DATE__ " " __TIME__);\
                __InitModule_ ## NAME(module);\
                Entity dataComponents[] = {0, ##__VA_ARGS__, 0};\
                for(auto i = 1; dataComponents[i]; ++i) {\
                    AddComponent(module, dataComponents[i]);\
                }\
                __InjectArrayPropertyElement(PropertyOf_Modules(), GetModuleRoot(), module);\
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
        AddComponent(component, ComponentOf_Component());\
        SetComponentSize(component, sizeof(COMPONENT));\
        __InjectArrayPropertyElement(PropertyOf_Components(), module, component);\
        SetName(component, #COMPONENT);\
        {\
            typedef COMPONENT ComponentType;

#define EndComponent() \
        }

#define RegisterNode(NAME) \
        node = NodeOf_ ## NAME ();\
        __InjectArrayPropertyElement(PropertyOf_Nodes(), module, node);\
        SetName(node, #NAME);

#define RegisterEvent(NAME) \
        event = EventOf_ ## NAME ();\
        __InjectArrayPropertyElement(PropertyOf_Events(), module, event);\
        SetName(event, #NAME);\
        SetEventArgsByDecl(event, __Event ## NAME);

#define RegisterProperty(PROPERTYTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
    __InjectArrayPropertyElement(PropertyOf_Properties(), component, property);\
    SetName(property, #PROPERTYNAME);\
    SetPropertyType(property, TypeOf_ ## PROPERTYTYPE);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyKind(property, TypeOf_ ## PROPERTYTYPE == TypeOf_StringRef ? PropertyKind_String : PropertyKind_Value);\
    event = EventOf_ ## PROPERTYNAME ## Changed ();\
    __InjectChildPropertyValue(PropertyOf_PropertyChangedEvent(), property, event);\
    SetName(event, #PROPERTYNAME "Changed");\
    SetEventArgsByDecl(event, __Event ## PROPERTYNAME ## Changed);

#define RegisterArrayProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
    __InjectArrayPropertyElement(PropertyOf_Properties(), component, property);\
    SetName(property, #PROPERTYNAME);\
    SetPropertyType(property, TypeOf_Entity);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyMeta(property, __ ## PROPERTYNAME ## __Meta);\
    SetPropertyChildComponent(property, ComponentOf_ ## COMPONENTTYPE());\
    SetPropertyKind(property, PropertyKind_Array);

#define RegisterChildProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
    __InjectArrayPropertyElement(PropertyOf_Properties(), component, property);\
    SetName(property, #PROPERTYNAME);\
    SetPropertyType(property, TypeOf_Entity);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyMeta(property, __ ## PROPERTYNAME ## __Meta);\
    SetPropertyChildComponent(property, ComponentOf_ ## COMPONENTTYPE());\
    SetPropertyKind(property, PropertyKind_Child);

#define RegisterBase(BASECOMPONENT) \
    base = GetArrayPropertyElement(PropertyOf_Bases(), component, AddArrayPropertyElement(PropertyOf_Bases(), component));\
    SetName(base, #BASECOMPONENT);\
    SetBaseComponent(base, ComponentOf_ ## BASECOMPONENT ());

#define RegisterExtension(BASECOMPONENT, EXTENSIONCOMPONENT) \
    extension = GetArrayPropertyElement(PropertyOf_Extensions(), module, AddArrayPropertyElement(PropertyOf_Extensions(), module));\
    SetName(extension, #BASECOMPONENT "_" #EXTENSIONCOMPONENT);\
    SetExtensionComponent(extension, ComponentOf_ ## BASECOMPONENT ());\
    SetExtensionExtenderComponent(extension, ComponentOf_ ## EXTENSIONCOMPONENT ());

#define RegisterFunction(FUNCTION) \
    function = FunctionOf_ ## FUNCTION ();\
    __InjectArrayPropertyElement(PropertyOf_Functions(), module, function);\
    SetName(function, #FUNCTION);

#define RegisterTest(TEST) \
    RegisterFunction(TEST)\
    AddComponent(function, ComponentOf_Test());

#define RegisterSubscription(EVENT, FUNCTION, SENDER) \
    snprintf(buffer, 1024, "%s_%s", unitName, #EVENT);\
    subscription = AddSubscriptions(module);\
    SetName(subscription, buffer);\
    subscriptionFunction = FunctionOf_ ## FUNCTION ();\
    SetSubscriptionEvent(subscription, EventOf_ ## EVENT ());\
    SetSubscriptionHandler(subscription, subscriptionFunction);\
    SetSubscriptionSender(subscription, SENDER);\
    if(!IsEntityValid(GetOwner(subscriptionFunction))) {\
        __InjectArrayPropertyElement(PropertyOf_Functions(), module, subscriptionFunction);\
    }

#define RegisterReferenceTracker(TRACKEDPROPERTY, REFERENCEPROPERTY) \
    RegisterFunction(On ## TRACKEDPROPERTY ## Changed)\
    RegisterFunction(On ## REFERENCEPROPERTY ## Changed)\
    RegisterSubscription(TRACKEDPROPERTY ## Changed, On ## TRACKEDPROPERTY ## Changed) \
    RegisterSubscription(REFERENCEPROPERTY ## Changed, On ## REFERENCEPROPERTY ## Changed)

#define RegisterChildCache(CHILDCOMPONENT) \
    RegisterFunction(On ## CHILDCOMPONENT ## ParentChanged)\
    RegisterSubscription(ParentChanged, On ## CHILDCOMPONENT ## ParentChanged, 0)

#define EndUnit() \
    }

#include <Core/Module.h>
#include <Core/Property.h>
#include <Core/Entity.h>
#include <Core/Component.h>
#include <Core/Event.h>

#endif //PLAZA_NATIVEUTILS_H