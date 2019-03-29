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

Entity ModuleOf_Core();

u32 GetComponentIndex(Entity component, Entity entity);
u32 GetComponentIndexByIndex(u32 componentInfoIndex, Entity entity);
Entity GetComponentEntity(u32 componentInfoIndex, u32 componentIndex);
const void* GetComponentInstanceData(u32 componentInfoIndex, u32 componentDataIndex);
void SetComponentInstanceData(u32 componentInfoIndex, u32 componentDataIndex, const void *data);

u32 GetArrayPropertyCount(Entity property, Entity entity);
void SetArrayPropertyCount(Entity property, Entity entity, u32 count);
u32 AddArrayPropertyElement(Entity property, Entity entity, Entity element);
u32 GetArrayPropertyIndex(Entity property, Entity entity, Entity element);
void RemoveArrayPropertyElement(Entity property, Entity entity, u32 index);
Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index);
const Entity* GetArrayPropertyElements(Entity property, Entity entity);

void SetModule(Entity entity, const struct Module& data);
void SetIdentification(Entity entity, const struct Identification& data);
void SetOwnership(Entity entity, const struct Ownership& data);
void SetFunction(Entity function, const struct Function& data);

const struct Identification& GetIdentification(Entity entity);
const struct Ownership& GetOwnership(Entity entity);
const struct Property& GetProperty(Entity entity);

Entity GetRoot();
Entity GetUnregisteredEntitiesRoot();

bool AddComponent(Entity entity, Entity type);
bool HasComponent(Entity entity, Entity component);

Entity AddEnumFlags(Entity entity);
void SetEnumFlagValue(Entity entity, u64 value);

StringRef Intern(StringRef str);
StringRef AddStringRef(StringRef sourceString);
void ReleaseStringRef(StringRef sourceString);

StringRef GetDebugName(Entity entity);
StringRef StringFormatV(StringRef format, ...);
void Log(Entity context, int severity, StringRef format, ...);
void Info(Entity context, StringRef format, ...);
void Warning(Entity context, StringRef format, ...);
void Error(Entity context, StringRef format, ...);

void SetFunctionArgsByDecl(Entity entity, StringRef decl);
void RegisterFunctionSignatureImpl(NativePtr invokerFuncPtr, StringRef signature);
void SetNativeFunctionInvokerBySignature(Entity function, StringRef signature);

void NotifyChange(Entity entity, Entity component, const void *oldData, const void *newData);

Variant CallNativeFunction(
        Entity f,
        u32 numArguments,
        const Variant *arguments
);

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
    inline const NAME& Get ## NAME(Entity entity) {\
        static auto componentInfoIndex = GetComponentIndexByIndex(0, ComponentOf_ ## NAME ());\
        auto componentIndex = GetComponentIndexByIndex(componentInfoIndex, entity);\
        return *(const NAME*)GetComponentInstanceData(componentInfoIndex, componentIndex);\
    }\
    inline void Set ## NAME(Entity entity, const NAME& data) {\
        static auto componentInfoIndex = GetComponentIndexByIndex(0, ComponentOf_ ## NAME ());\
        auto componentIndex = GetComponentIndexByIndex(componentInfoIndex, entity);\
        SetComponentInstanceData(componentInfoIndex, componentIndex, (const char*)&data);\
    }


#define Unit(NAME) \
    void __InitUnit_ ## NAME (Entity module);

#define Module(NAME) \
    Entity ModuleOf_ ## NAME ();

#define Enum(NAME) \
    Declare(Enum, NAME)\

#define Property(PROPERTYTYPE, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME)

#define ReferenceProperty(REFERENCECOMPONENT, PROPERTYNAME) \
    Property(Entity, PROPERTYNAME)

#define ChildProperty(CHILDCOMPONENT, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME)

#define ArrayProperty(ELEMENTCOMPONENT, PROPERTYNAME) \
    Declare(Property, PROPERTYNAME)\
    inline const Entity* Get ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        return GetArrayPropertyElements(prop, entity);\
    }\
    inline u32 Add ## PROPERTYNAME(Entity entity, Entity element) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        return AddArrayPropertyElement(prop, entity, element);\
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
    inline u32 GetNum ## PROPERTYNAME(Entity entity) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        return GetArrayPropertyCount(prop, entity);\
    }\
    inline void SetNum ## PROPERTYNAME(Entity entity, u32 count) {\
        static Entity prop = PropertyOf_ ## PROPERTYNAME();\
        SetArrayPropertyCount(prop, entity, count);\
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
				SetIdentification(entity, {"Function." #NAME FILE });\
				Function functionData;\
				functionData.FunctionReturnType = TypeOf_ ## R;\
                functionData.FunctionPtr = (NativePtr)& NAME;\
                SetFunction(entity, functionData);\
                SetFunctionArgsByDecl(entity, #__VA_ARGS__);\
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
                Module data;\
                data.ModuleSourcePath = __FILE__;\
                data.ModuleVersion = __DATE__ " " __TIME__;\
                SetModule(module, data);\
				SetIdentification(module, {"Module." #NAME});\
				SetOwnership(module, {GetRoot(), PropertyOf_Modules()});\
				auto moduleRootData = GetModuleRoot(GetRoot());\
				moduleRootData.Modules.push_back(module);\
				SetModuleRoot(GetRoot(), moduleRootData);\
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
    }

#define BeginUnit(NAME) \
    void __InitUnit_ ## NAME (Entity module) {\
        static bool initialized = false;\
        if(initialized) return;\
        initialized = true;\
        Component componentData;\
        System systemData;\
        Property propertyData;\
        Enum enumData;\
        Entity node = 0, type = 0, component = 0, property = 0, event = 0, argument = 0, argumentParent = 0, base = 0, extension = 0, function = 0, subscription = 0, subscriptionFunction = 0, eenum = 0, flag = 0;\
        auto unitName = #NAME;

#define BeginComponent(COMPONENT) \
        component = ComponentOf_ ## COMPONENT ();\
        SetArrayChild(component, {#COMPONENT});\
        AddComponents(module, component);\
		SetIdentification(component, {"Component." #COMPONENT});\
		componentData = GetComponent(component);\
		componentData.ComponentExplicitSize = true;\
		componentData.ComponentSize = sizeof(COMPONENT);\
		SetComponent(component, componentData);\
        {\
            typedef COMPONENT ComponentType;

#define EndComponent() \
        }

#define RegisterSystem(NAME, COMPONENT) \
        {\
            auto system = CreateEntity();\
            SetArrayChild(system, {#NAME});\
            AddSystems(module, system);\
            systemData = GetSystem(system);\
            systemData.SystemFunction = (NativePtr)&NAME;\
            systemData.SystemOrder = 0;\
            systemData.SystemDeferred = false;\
            systemData.SystemComponent = COMPONENT;\
            SetSystem(system, systemData);\
        }

#define RegisterDeferredSystem(NAME, COMPONENT, ORDER) \
        {\
            auto system = CreateEntity();\
            SetArrayChild(system, {#NAME});\
            AddSystems(module, system);\
            systemData = GetSystem(system);\
            systemData.SystemFunction = (NativePtr)&NAME;\
            systemData.SystemOrder = ORDER;\
            systemData.SystemDeferred = true;\
            systemData.SystemComponent = COMPONENT;\
            SetSystem(system, systemData);\
        }

#define RegisterProperty(PROPERTYTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
    SetArrayChild(property, {#PROPERTYNAME});\
    AddProperties(component, property);\
    propertyData = GetProperty(property);\
    propertyData.PropertyType = TypeOf_ ## PROPERTYTYPE;\
    propertyData.PropertyOffset = offsetof(ComponentType, PROPERTYNAME);\
    propertyData.PropertySize = sizeof(ComponentType::PROPERTYNAME);\
    SetProperty(property, propertyData);

#define RegisterPropertyEnum(PROPERTYTYPE, PROPERTYNAME, ENUM) \
    RegisterProperty(PROPERTYTYPE, PROPERTYNAME) \
    propertyData.PropertyEnum = EnumOf_ ## ENUM ();\
    SetProperty(property, propertyData);


#define RegisterPropertyReadOnly(PROPERTYTYPE, PROPERTYNAME) \
    RegisterProperty(PROPERTYTYPE, PROPERTYNAME) \
    propertyData.PropertyReadOnly = true;\
    SetProperty(property, propertyData);

#define RegisterArrayProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetArrayChild(property, {#PROPERTYNAME});\
	AddProperties(component, property);\
    propertyData = GetProperty(property);\
    propertyData.PropertyType = TypeOf_ChildArray;\
    propertyData.PropertyOffset = offsetof(ComponentType, PROPERTYNAME);\
    propertyData.PropertyChildComponent = ComponentOf_ ## COMPONENTTYPE();\
    SetProperty(property, propertyData);

#define RegisterChildProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetArrayChild(property, {#PROPERTYNAME});\
	AddProperties(component, property);\
    propertyData = GetProperty(property);\
    propertyData.PropertyType = TypeOf_Entity;\
    propertyData.PropertyOffset = offsetof(ComponentType, PROPERTYNAME);\
    propertyData.PropertyChildComponent = ComponentOf_ ## COMPONENTTYPE();\
    SetProperty(property, propertyData);


#define RegisterReferenceProperty(COMPONENTTYPE, PROPERTYNAME)\
    property = PropertyOf_ ## PROPERTYNAME ();\
	SetArrayChild(property, {#PROPERTYNAME});\
	AddProperties(component, property);\
    propertyData = GetProperty(property);\
    propertyData.PropertyType = TypeOf_Entity;\
    propertyData.PropertyOffset = offsetof(ComponentType, PROPERTYNAME);\
    SetProperty(property, propertyData);

#define RegisterReferencePropertyReadOnly(COMPONENTTYPE, PROPERTYNAME)\
    RegisterReferenceProperty(COMPONENTTYPE, PROPERTYNAME)\
    propertyData.PropertyReadOnly = true;\
    SetProperty(property, propertyData);

#define RegisterArrayPropertyReadOnly(COMPONENTTYPE, PROPERTYNAME)\
    RegisterArrayProperty(COMPONENTTYPE, PROPERTYNAME)\
    propertyData.PropertyReadOnly = true;\
    SetProperty(property, propertyData);


#define RegisterBase(BASECOMPONENT) \
    {\
        auto base = CreateEntity();\
        SetArrayChild(base, {#BASECOMPONENT});\
        AddBases(component, base);\
        SetBase(base, {ComponentOf_ ## BASECOMPONENT ()});\
    }

#define RegisterExtension(BASECOMPONENT, EXTENSIONCOMPONENT) \
    extension = CreateEntity();\
    SetArrayChild(extension, #EXTENSIONCOMPONENT);\
    AddExtensions(module, extension);\
    SetExtension(extension, {ComponentOf_ ## BASECOMPONENT (), ComponentOf_ ## EXTENSIONCOMPONENT ()});

#define RegisterFunction(FUNCTION) \
    function = FunctionOf_ ## FUNCTION ();\
    SetArrayChild(function, {#FUNCTION});\
    AddFunctions(module, function);

#define BeginEnum(ENUM, COMBINABLE) \
    eenum = EnumOf_ ## ENUM ();\
    SetArrayChild(eenum, {#ENUM});\
    AddEnums(module, eenum);\
    enumData = GetEnum(eenum);\
	enumData.EnumCombinable = COMBINABLE;

#define RegisterFlag(FLAG) \
    flag = CreateEntity();\
    SetArrayChild(flag, {#FLAG});\
    AddEnumFlags(eenum, flag);\
    SetEnumFlag(flag, {FLAG});\
    enumData.EnumFlags.push_back(flag);

#define EndEnum()

#define EndUnit() \
    }

#define for_children(CHILD, PROPERTY, PARENT) \
    for(Entity CHILD ## _index = 0, CHILD = GetArrayPropertyElement(PROPERTY, PARENT, (u32)CHILD ## _index); CHILD ## _index < GetArrayPropertyCount(PROPERTY, PARENT); ++CHILD ## _index)

#include <Core/Identification.h>
#include <Core/Module.h>
#include <Core/Function.h>
#include <Core/Property.h>
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Enum.h>
#include <Core/System.h>

#endif //PLAZA_NATIVEUTILS_H
