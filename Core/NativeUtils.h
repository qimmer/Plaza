//
// Created by Kim on 07/06/2018.
//

#ifndef PLAZA_NATIVEUTILS_H
#define PLAZA_NATIVEUTILS_H

#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#include <Core/Types.h>
#include <Core/Variant.h>
#include <Core/Vector.h>

void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line);


typedef u64 Entity;

typedef bool(*FunctionCaller)(u64 functionImplementation, Type returnArgumentTypeIndex, void *returnData, u32 numArguments, const Type *argumentTypes, const void **argumentDataPtrs);

typedef eastl::fixed_vector<Entity, 8> ChildArray;

#define PropertyKind_Value 0
#define PropertyKind_Child 1
#define PropertyKind_Array 2

Entity ModuleOf_Core();

u32 GetArrayPropertyCount(Entity property, Entity entity);
bool SetArrayPropertyCount(Entity property, Entity entity, u32 count);
u32 AddArrayPropertyElement(Entity property, Entity entity);
u32 GetArrayPropertyIndex(Entity property, Entity entity, Entity element);
bool RemoveArrayPropertyElement(Entity property, Entity entity, u32 index);
Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index);
const ChildArray& GetArrayPropertyElements(Entity property, Entity entity);

void SetModuleSourcePath(Entity module, StringRef sourcePath);
void SetModuleVersion(Entity module, StringRef version);
Entity GetModuleRoot();
Entity GetUnregisteredEntitiesRoot();
StringRef GetUuid(Entity entity);
void SetUuid(Entity entity, StringRef uuid);
void SetOwner(Entity entity, Entity owner, Entity ownerProperty);
bool AddComponent(Entity entity, Entity type);
bool HasComponent(Entity entity, Entity component);
void SetComponentSize(Entity entity, u16 size);
void SetEventArgsByDecl(Entity entity, StringRef decl);
void SetEventArgumentOffset(Entity entity, u32 offset);
void SetEventArgumentType(Entity entity, Type type);
Entity AddEnumFlags(Entity entity);
void SetEnumFlagValue(Entity entity, u64 value);
void SetPropertyType(Entity entity, Type type);
void SetPropertyValue(Entity entity, Entity context, Variant valueData);
void SetPropertyOffset(Entity entity, u32 offset);
u32 GetPropertyOffset(Entity entity);
struct Property* GetPropertyData(Entity entity);
Type GetPropertyType(Entity entity);
void SetPropertySize(Entity entity, u32 size);
void SetPropertyChildComponent(Entity entity, Entity component);
void SetPropertyKind(Entity property, u8 kind);
Entity GetOwner(Entity entity);
StringRef Intern(StringRef str);
StringRef AddStringRef(StringRef sourceString);
void ReleaseStringRef(StringRef sourceString);
StringRef GetDebugName(Entity entity);
StringRef StringFormatV(StringRef format, ...);
void Log(Entity context, int severity, StringRef format, ...);
void Info(Entity context, StringRef format, ...);
void Warning(Entity context, StringRef format, ...);
void Error(Entity context, StringRef format, ...);
void FireEventFast(
        Entity event,
        u32 numArguments,
        const Variant *arguments
);
void SetEventSize(Entity event, u32 size);

void SetFunctionPtr(Entity function, NativePtr func);
void SetFunctionCaller(Entity entity, NativePtr callerFunc);
void SetFunctionReturnType(Entity entity, Type returnType);
void SetFunctionArgsByDecl(Entity entity, StringRef decl);
void RegisterFunctionSignatureImpl(NativePtr invokerFuncPtr, StringRef signature);
void SetNativeFunctionInvokerBySignature(Entity function, StringRef signature);


Variant CallNativeFunction(
        Entity f,
        u32 numArguments,
        const Variant *arguments
);

Entity EventOf_ParentChanged();

Entity GetUniqueEntity(StringRef name, bool *firstTime);
StringRef GetUniqueEntityName(Entity entity);

// Declaration Macros
#define GetEntityIndex(handle) (((u32 *) &handle)[0])

#define Declare(TYPE, NAME) \
    inline Entity TYPE ## Of_ ## NAME () {\
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(entity == 0) {\
            entity = GetUniqueEntity(#TYPE "." #NAME, &firstTime);\
        }\
        return entity;\
    }

#define Component(NAME) \
    struct NAME;\
    Declare(Component, NAME)\
    inline NAME * Get ## NAME ## Data(Entity entity) {\
        static auto component = ComponentOf_ ## NAME ();\
        static auto type = GetComponentType(component);\
        auto componentIndex = _GetComponentIndex(type, GetEntityIndex(entity));\
		if(componentIndex == InvalidIndex) return NULL;\
        return (NAME*)_GetComponentData(type, componentIndex);\
    }


#define Unit(NAME) \
    void __InitUnit_ ## NAME (Entity module);

#define Module(NAME) \
    Entity ModuleOf_ ## NAME ();

#define Event(NAME, ...) \
    static const StringRef __Event ## NAME = (#__VA_ARGS__ "");\
    Declare(Event, NAME)

#define Enum(NAME) \
    Declare(Enum, NAME)\

#define __PropertyCoreGetOnly(COMPONENT, PROPERTYTYPE, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = (#__VA_ARGS__ "");\
    PROPERTYTYPE Get ## PROPERTYNAME(Entity entity);\

#define __PropertyCore(COMPONENT, PROPERTYTYPE, PROPERTYNAME, ...) \
    __PropertyCoreGetOnly(COMPONENT, PROPERTYTYPE, PROPERTYNAME, ##__VA_ARGS__ )\
    void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE value);


#define __PropertyCoreGet(TYPE, PROPERTY, COMPONENT) \
    API_EXPORT TYPE Get ## PROPERTY(Entity entity) {\
        if(!IsEntityValid(entity)) return 0;\
        auto data = Get ## COMPONENT ## Data(entity);\
        if(data) return data-> PROPERTY;\
        return 0;\
    }

#define __PropertyCoreSet(TYPE, PROPERTY, COMPONENT) \
    API_EXPORT void Set ## PROPERTY (Entity entity, TYPE value) {\
        if(!IsEntityValid(entity)) return;\
        AddComponent(entity, ComponentOf_ ## COMPONENT ());\
        Get ## COMPONENT ## Data(entity)->PROPERTY = value;\
    }

#define __PropertyCoreImpl(TYPE, PROPERTY, COMPONENT) \
    __PropertyCoreGet(TYPE, PROPERTY, COMPONENT)\
    __PropertyCoreSet(TYPE, PROPERTY, COMPONENT)


#define Property(PROPERTYTYPE, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME) \
    inline PROPERTYTYPE Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        static u32 offset = GetPropertyOffset(prop);\
        static Entity component = GetOwner(prop);\
        static auto type = GetComponentType(component);\
        \
        auto componentIndex = _GetComponentIndex(type, GetEntityIndex(entity));\
        if(componentIndex == InvalidIndex) return PROPERTYTYPE ## _Default;\
        auto componentData = _GetComponentData(type, componentIndex);\
        return *((PROPERTYTYPE*)(componentData + offset));\
    }\
    inline void Set ## PROPERTYNAME(Entity entity, PROPERTYTYPE newValue) {\
        static Entity property = PropertyOf_ ## PROPERTYNAME();\
		static auto propertyData = GetPropertyData(property);\
        static u32 offset = propertyData->PropertyOffset;\
        static Entity component = GetOwner(property);\
        static auto type = GetComponentType(component);\
        static auto size = sizeof(PROPERTYTYPE);\
		const auto isString = propertyData->PropertyType == TypeOf_StringRef;\
		const auto isVariant = propertyData->PropertyType == TypeOf_Variant;\
        \
        auto componentIndex = _GetComponentIndex(type, GetEntityIndex(entity));\
        \
        if(componentIndex == InvalidIndex) {\
            AddComponent(entity, component);\
            componentIndex = _GetComponentIndex(type, GetEntityIndex(entity));\
        }\
        \
        auto componentData = _GetComponentData(type, componentIndex);\
        auto valueData = componentData + offset;\
        \
        if(isString || (isVariant && ((Variant*)valueData)->type == TypeOf_StringRef)) {\
            ReleaseStringRef(*(StringRef*)valueData);\
        }\
        \
        if(isString || (isVariant && ((Variant*)&newValue)->type == TypeOf_StringRef)) {\
            *(StringRef*)&newValue = AddStringRef(*(StringRef*)&newValue);\
        }\
        \
        if(memcmp(valueData, &newValue, size) != 0) {\
            Variant oldValueData, newValueVar;\
            memcpy(&oldValueData, valueData, size);\
			memcpy(&newValueVar, &newValue, size);\
			if(!isVariant) {\
				oldValueData.type = propertyData->PropertyType;\
				newValueVar.type = propertyData->PropertyType;\
			}\
            memcpy(valueData, &newValue, size);\
            \
            auto& cache = GetSubscriptionCache(propertyData->PropertyChangedEvent);\
            \
            Variant arguments[] = { MakeVariant(Entity, entity), oldValueData, newValueVar};\
            \
            for(auto i = 0; i < cache.size(); ++i) {\
                auto& subscription = cache[i];\
                if(!subscription.SubscriptionSender || subscription.SubscriptionSender == entity) {\
                    auto functionData = GetFunctionData(subscription.SubscriptionHandler);\
                    \
                    ((FunctionCallerType)functionData->FunctionCaller)(\
                        functionData->FunctionPtr,\
                        arguments\
                    );\
                }\
            }\
            \
            static auto& listeners = GetGenericPropertyChangedListeners();\
            for(auto listener : listeners) {\
                listener(property, entity, propertyData->PropertyType, oldValueData, newValueVar);\
            }\
        }\
    }

#define ReferenceProperty(REFERENCECOMPONENT, PROPERTYNAME) \
    Property(Entity, PROPERTYNAME)

#define ChildProperty(CHILDCOMPONENT, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME) \
    inline Entity Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        static u32 offset = GetPropertyOffset(prop);\
        static Entity component = GetOwner(prop);\
        static auto type = GetComponentType(component);\
        \
        auto componentIndex = _GetComponentIndex(type, GetEntityIndex(entity));\
        if(componentIndex == InvalidIndex) return Entity_Default;\
        auto componentData = _GetComponentData(type, componentIndex);\
        return *((Entity*)(componentData + offset));\
    }


#define __ChildPropertyCore(CHILDCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = (#__VA_ARGS__ "");\
    Entity Get ## PROPERTYNAME(Entity entity);


#define __ChildPropertyCoreImpl(TYPE, PROPERTY, COMPONENT) \
    API_EXPORT TYPE Get ## PROPERTY(Entity entity) {\
        auto data = Get ## COMPONENT ## Data(entity);\
        if(data) return data-> PROPERTY;\
        return 0;\
    }


#define ArrayProperty(ELEMENTCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = (#__VA_ARGS__ "");\
    inline const ChildArray& Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        return GetArrayPropertyElements(prop, entity);\
    }\
    inline Entity Add ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        auto index = AddArrayPropertyElement(prop, entity);\
        return GetArrayPropertyElement(prop, entity, index);\
    }\
    inline void Remove ## PROPERTYNAME(Entity entity, u32 index) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        RemoveArrayPropertyElement(prop, entity, index);\
    }\
    inline void Remove ## PROPERTYNAME ## ByValue(Entity entity, Entity value) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        auto index = GetArrayPropertyIndex(prop, entity, value);\
        RemoveArrayPropertyElement(prop, entity, index);\
    }\
    inline u32 Get ## PROPERTYNAME ## Index(Entity entity, Entity element) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        return GetArrayPropertyIndex(prop, entity, element);\
    }\
    inline void SetNum ## PROPERTYNAME(Entity entity, u32 count) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        SetArrayPropertyCount(prop, entity, count);\
    }


#define __ArrayPropertyCore(ELEMENTCOMPONENT, PROPERTYNAME, ...) \
    Declare(Property, PROPERTYNAME) \
    static const StringRef __ ## PROPERTYNAME ## __Meta = (#__VA_ARGS__ "");\
    const ChildArray& Get ## PROPERTYNAME(Entity entity);\
    Entity Add ## PROPERTYNAME(Entity entity);\
    inline void Remove ## PROPERTYNAME(Entity entity, u32 index) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        RemoveArrayPropertyElement(prop, entity, index);\
    }

#define __ArrayPropertyCoreImpl(ELEMENTCOMPONENT, PROPERTYNAME, COMPONENT) \
    API_EXPORT const ChildArray& Get ## PROPERTYNAME(Entity entity) {\
        return GetChildArray(PropertyOf_ ## PROPERTYNAME(), entity);\
    }\
    API_EXPORT Entity Add ## PROPERTYNAME(Entity entity) {\
        auto element = CreateEntity();\
        auto index = AddChild(PropertyOf_ ## PROPERTYNAME (), entity, element, true);\
        AddComponent(element, ComponentOf_ ## ELEMENTCOMPONENT ());\
        return element;\
    }


#define RegisterFunctionSignature(Function, ...) \
    RegisterFunctionSignatureImpl((NativePtr)Function, #__VA_ARGS__);

#define __NFI_Typedef(ReturnType, ...)\
    typedef ReturnType(*FunctionType)(__VA_ARGS__);\
    auto funcPtr = (FunctionType)genericFuncPtr;\

#define __NFI_InvokeVoid(...)\
    funcPtr(__VA_ARGS__);\
    return Variant_Empty;

#define __NFI_Invoke(ReturnType, ...)\
    auto returnValue = funcPtr(__VA_ARGS__);\
    return __MakeVariant(&returnValue, TypeOf_ ## ReturnType);

#define __NFI_Arg(TYPE, INDEX) \
    *(TYPE*)(&arguments[INDEX].data)

#define NativeFunctionInvokerVoid0() \
    inline Variant NativeFunctionInvoker_void (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(void)\
        __NFI_InvokeVoid()\
    }

#define NativeFunctionInvoker0(ReturnType) \
    inline Variant NativeFunctionInvoker_ ## ReturnType (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(ReturnType)\
        __NFI_Invoke(ReturnType)\
    }

#define NativeFunctionInvokerVoid1(ArgType0) \
    inline Variant NativeFunctionInvoker_void_ ## ArgType0 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(void, ArgType0)\
        __NFI_InvokeVoid(__NFI_Arg(ArgType0, 0))\
    }

#define NativeFunctionInvoker1(ReturnType, ArgType0) \
    inline Variant NativeFunctionInvoker_ ## ReturnType ## _ ## ArgType0 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(ReturnType, ArgType0)\
        __NFI_Invoke(ReturnType, __NFI_Arg(ArgType0, 0))\
    }

#define NativeFunctionInvokerVoid2(ArgType0, ArgType1) \
    inline Variant NativeFunctionInvoker_void_ ## ArgType0 ## _ ## ArgType1 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(void, ArgType0, ArgType1)\
        __NFI_InvokeVoid(__NFI_Arg(ArgType0, 0), __NFI_Arg(ArgType1, 1))\
    }

#define NativeFunctionInvoker2(ReturnType, ArgType0, ArgType1) \
    inline Variant NativeFunctionInvoker_ ## ReturnType ## _ ## ArgType0 ## _ ## ArgType1 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(ReturnType, ArgType0, ArgType1)\
        __NFI_Invoke(ReturnType, __NFI_Arg(ArgType0, 0), __NFI_Arg(ArgType1, 1))\
    }

#define NativeFunctionInvokerVoid3(ArgType0, ArgType1, ArgType2) \
    inline Variant NativeFunctionInvoker_void_ ## ArgType0 ## _ ## ArgType1 ## _ ## ArgType2 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(void, ArgType0, ArgType1, ArgType2)\
        __NFI_InvokeVoid(__NFI_Arg(ArgType0, 0), __NFI_Arg(ArgType1, 1), __NFI_Arg(ArgType2, 2))\
    }

#define NativeFunctionInvoker3(ReturnType, ArgType0, ArgType1, ArgType2) \
    inline Variant NativeFunctionInvoker_ ## ReturnType ## _ ## ArgType0 ## _ ## ArgType1 ## _ ## ArgType2 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(ReturnType, ArgType0, ArgType1, ArgType2)\
        __NFI_Invoke(ReturnType, __NFI_Arg(ArgType0, 0), __NFI_Arg(ArgType1, 1), __NFI_Arg(ArgType2, 2))\
    }

#define NativeFunctionInvokerVoid4(ArgType0, ArgType1, ArgType2, ArgType3) \
    inline Variant NativeFunctionInvoker_void_ ## ArgType0 ## _ ## ArgType1 ## _ArgType2 ## _ ## ArgType3 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(void, ArgType0, ArgType1, ArgType2, ArgType3)\
        __NFI_InvokeVoid(__NFI_Arg(ArgType0, 0), __NFI_Arg(ArgType1, 1), __NFI_Arg(ArgType2, 2), __NFI_Arg(ArgType3, 3))\
    }

#define NativeFunctionInvoker4(ReturnType, ArgType0, ArgType1, ArgType2, ArgType3) \
    inline Variant NativeFunctionInvoker_ ## ReturnType ## _ ## ArgType0 ## _ ## ArgType1 ## _ ## ArgType2 ## _ ## ArgType3 (void *genericFuncPtr, const Variant *arguments) {\
        __NFI_Typedef(ReturnType, ArgType0, ArgType1, ArgType2, ArgType3)\
        __NFI_Invoke(ReturnType, __NFI_Arg(ArgType0, 0), __NFI_Arg(ArgType1, 1), __NFI_Arg(ArgType2, 2), __NFI_Arg(ArgType3, 3))\
    }

#define __Function(FILE, NAME, R, ...) \
        static Entity entity = 0;\
        static bool firstTime = false;\
        if(!entity) {\
            entity = GetUniqueEntity("Function." #NAME FILE, &firstTime);\
            if(firstTime) {\
                char unregisteredName[64];\
                StringRef lastSlash = strrchr(FILE, '/');\
                StringRef lastBackSlash = strrchr(FILE, '\\');\
                if(lastBackSlash && lastSlash && (lastBackSlash - FILE) > (lastSlash - FILE)) lastSlash = lastBackSlash;\
                if(!lastSlash) lastSlash = lastBackSlash;\
                if(!lastSlash) lastSlash = FILE;\
                snprintf(unregisteredName, 64, "%s:" #NAME, lastSlash + 1);\
				SetUuid(entity, "Function." #NAME FILE);\
                SetFunctionReturnType(entity, TypeOf_ ## R);\
                SetFunctionArgsByDecl(entity, #__VA_ARGS__);\
                SetFunctionPtr(entity, (NativePtr)& NAME );\
                SetNativeFunctionInvokerBySignature(entity, #R ", " #__VA_ARGS__);\
            }\
        }\
        return entity;

#define Function(NAME, R, ...) \
    R NAME( __VA_ARGS__ ); \
    inline Entity FunctionOf_ ## NAME () {\
        __Function("", NAME, R, ##__VA_ARGS__)\
    }

#define Test(NAME) \
    Function(NAME, s32, Entity test)\
    s32 NAME (Entity test)

#define LocalFunction(NAME, R, ...) \
    static R NAME(__VA_ARGS__);\
    static Entity FunctionOf_ ## NAME () {\
        __Function("." __FILE__, NAME, R, __VA_ARGS__) \
    }\
    static R NAME(__VA_ARGS__)


#define BeginModule(NAME) \
    void __InitModule_ ## NAME(Entity module);\
    API_EXPORT Entity ModuleOf_ ## NAME () {\
        static Entity module = 0;\
        static bool firstTime = false;\
        if(!module) {\
            ModuleOf_Core();\
            module = GetUniqueEntity("Module." #NAME, &firstTime);\
            if(firstTime) {\
                SetModuleSourcePath(module, __FILE__);\
                SetModuleVersion(module, __DATE__ " " __TIME__);\
				SetUuid(module, "Module." #NAME);\
                AddChild(PropertyOf_Modules(), GetModuleRoot(), module, true);\
                auto moduleVar = MakeVariant(Entity, module);\
                FireEventFast(EventOf_ModuleLoadStarted(), 1, &moduleVar);\
                __InitModule_ ## NAME(module);\
                FireEventFast(EventOf_ModuleLoadFinished(), 1, &moduleVar);\
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
    }

#define BeginUnit(NAME) \
    void __InitUnit_ ## NAME (Entity module) {\
        static bool initialized = false;\
        if(initialized) return;\
        initialized = true;\
        Entity node = 0, type = 0, component = 0, property = 0, event = 0, argument = 0, argumentParent = 0, base = 0, extension = 0, function = 0, subscription = 0, subscriptionFunction = 0, eenum = 0, flag = 0;\
        auto unitName = #NAME;\
        char buffer[1024];\
		memset(buffer, 0, sizeof(buffer));

#define BeginComponent(COMPONENT) \
        component = ComponentOf_ ## COMPONENT ();\
		SetUuid(component, "Component." #COMPONENT);\
        AddComponent(component, ComponentOf_Component());\
        SetComponentExplicitSize(component, true);\
        SetComponentSize(component, sizeof(COMPONENT));\
        AddChild(PropertyOf_Components(), module, component, true);\
        {\
            typedef COMPONENT ComponentType;

#define EndComponent() \
        }

#define RegisterEvent(NAME) \
        event = EventOf_ ## NAME ();\
		SetUuid(event, "Event." #NAME);\
        AddChild(PropertyOf_Events(), module, event, true);\
        SetEventArgsByDecl(event, __Event ## NAME);

#define RegisterProperty(PROPERTYTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetUuid(property, "Property." #PROPERTYNAME);\
    AddChild(PropertyOf_Properties(), component, property, true);\
    SetPropertyType(property, TypeOf_ ## PROPERTYTYPE);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyKind(property, PropertyKind_Value);\
    event = GetPropertyChangedEvent(property);\
    if(!event) { event = GetPropertyData(property)->PropertyChangedEvent = CreateEntity(); SetUuid(event, StringFormatV("%s.PropertyChangedEvent", GetUuid(property))); SetOwner(event, property, PropertyOf_PropertyChangedEvent()); }\
    SetEventArgsByDecl(event, "Entity entity, " #PROPERTYTYPE " oldValue, " #PROPERTYTYPE " newValue");

#define RegisterPropertyEnum(PROPERTYTYPE, PROPERTYNAME, ENUM) \
    RegisterProperty(PROPERTYTYPE, PROPERTYNAME) \
    SetPropertyEnum(property, EnumOf_ ## ENUM ());


#define RegisterPropertyReadOnly(PROPERTYTYPE, PROPERTYNAME) \
    RegisterProperty(PROPERTYTYPE, PROPERTYNAME) \
    SetPropertyReadOnly(property, true);

#define RegisterArrayProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetUuid(property, "Property." #PROPERTYNAME);\
    AddChild(PropertyOf_Properties(), component, property, true);\
    SetPropertyType(property, TypeOf_Entity);\
    SetPropertyOffset(property, InvalidIndex);\
    SetPropertyChildComponent(property, ComponentOf_ ## COMPONENTTYPE());\
    SetPropertyKind(property, PropertyKind_Array);\
    event = GetPropertyChangedEvent(property);\
    if(!event) { event = GetPropertyData(property)->PropertyChangedEvent = CreateEntity(); SetUuid(event, StringFormatV("%s.PropertyChangedEvent", GetUuid(property))); SetOwner(event, property, PropertyOf_PropertyChangedEvent()); }\
    SetEventArgsByDecl(event, "Entity entity, Entity oldValue, Entity newValue");

#define RegisterChildProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetUuid(property, "Property." #PROPERTYNAME);\
    AddChild(PropertyOf_Properties(), component, property, true);\
    SetPropertyType(property, TypeOf_Entity);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyChildComponent(property, ComponentOf_ ## COMPONENTTYPE());\
    SetPropertyKind(property, PropertyKind_Child);


#define RegisterReferenceProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetUuid(property, "Property." #PROPERTYNAME);\
    AddChild(PropertyOf_Properties(), component, property, true);\
    SetPropertyType(property, TypeOf_Entity);\
    SetPropertyOffset(property, offsetof(ComponentType, PROPERTYNAME));\
    SetPropertySize(property, sizeof(ComponentType::PROPERTYNAME));\
    SetPropertyChildComponent(property, ComponentOf_ ## COMPONENTTYPE());\
    SetPropertyKind(property, PropertyKind_Value);\
    event = GetPropertyChangedEvent(property);\
    if(!event) { event = GetPropertyData(property)->PropertyChangedEvent = CreateEntity(); SetUuid(event, StringFormatV("%s.PropertyChangedEvent", GetUuid(property))); SetOwner(event, property, PropertyOf_PropertyChangedEvent()); }\
    SetEventArgsByDecl(event, "Entity entity, Entity oldValue, Entity newValue");

#define RegisterReferencePropertyReadOnly(COMPONENTTYPE, PROPERTYNAME)\
    RegisterReferenceProperty(COMPONENTTYPE, PROPERTYNAME)\
    SetPropertyReadOnly(property, true);

#define RegisterArrayPropertyReadOnly(COMPONENTTYPE, PROPERTYNAME)\
    RegisterArrayProperty(COMPONENTTYPE, PROPERTYNAME)\
    SetPropertyReadOnly(property, true);


#define RegisterBase(BASECOMPONENT) \
    {\
        auto base = AddBases(component);\
        snprintf(buffer, 1024, "%s.Bases.%s", GetUuid(component), #BASECOMPONENT);\
        SetUuid(base, buffer);\
        SetBaseComponent(base, ComponentOf_ ## BASECOMPONENT ());\
    }

#define RegisterExtension(BASECOMPONENT, EXTENSIONCOMPONENT) \
    extension = GetArrayPropertyElement(PropertyOf_Extensions(), module, AddArrayPropertyElement(PropertyOf_Extensions(), module));\
    snprintf(buffer, 1024, "%s.Extensions.%s", GetUuid(module), #EXTENSIONCOMPONENT);\
    SetUuid(extension, buffer);\
    SetExtensionComponent(extension, ComponentOf_ ## BASECOMPONENT ());\
    SetExtensionExtenderComponent(extension, ComponentOf_ ## EXTENSIONCOMPONENT ());

#define RegisterFunction(FUNCTION) \
    function = FunctionOf_ ## FUNCTION ();\
	SetUuid(function, "Function." #FUNCTION);\
    AddChild(PropertyOf_Functions(), module, function, true);

#define RegisterSubscription(EVENT, FUNCTION, SENDER) \
    snprintf(buffer, 1024, "%s.%s.Subscriptions.%s.%s", GetUuid(module), unitName, #EVENT, #FUNCTION);\
    subscription = AddSubscriptions(module);\
    SetUuid(subscription, buffer);\
    subscriptionFunction = FunctionOf_ ## FUNCTION ();\
    SetSubscriptionSender(subscription, SENDER);\
    SetSubscriptionEvent(subscription, EVENT);\
    SetSubscriptionHandler(subscription, subscriptionFunction);\
    if(!IsEntityValid(GetOwner(subscriptionFunction))) {\
        AddChild(PropertyOf_Functions(), module, subscriptionFunction, true);\
    }

#define BeginEnum(ENUM, COMBINABLE) \
    eenum = EnumOf_ ## ENUM ();\
	SetUuid(eenum, "Enum." #ENUM);\
    AddChild(PropertyOf_Enums(), module, eenum, true);\
    SetEnumCombinable(eenum, (COMBINABLE));

#define RegisterFlag(FLAG) \
    snprintf(buffer, 1024, "%s.%s", GetUuid(eenum), #FLAG);\
    flag = AddEnumFlags(eenum);\
    SetUuid(flag, buffer);\
    SetEnumFlagValue(flag, (FLAG));

#define EndEnum()

#define EndUnit() \
    }

#define for_children(VARNAME, PROPERTY, PARENTENTITY) \
        const auto& _entries ## VARNAME = Get ## PROPERTY (PARENTENTITY);\
        for(Entity VARNAME : _entries ## VARNAME)

#define for_children_abstract(VARNAME, PROPERTY, PARENTENTITY) \
        u32 _count ## VARNAME = 0, _i ## VARNAME = 0;\
        const auto& _entries ## VARNAME = GetArrayPropertyElements(PROPERTY, PARENTENTITY);\
        for(Entity VARNAME : _entries ## VARNAME)

#include <Core/Event.h>
#include <Core/Property.h>
#include <Core/Function.h>
#include <Core/Component.h>
#include <Core/Module.h>
#include <Core/Entity.h>
#include <Core/Enum.h>

#endif //PLAZA_NATIVEUTILS_H
