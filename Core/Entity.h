#ifndef PLAZA_ENTITY_H
#define PLAZA_ENTITY_H

#include <Core/Type.h>
#include <Core/Property.h>
#include <Core/Handle.h>
#include <Core/Pool.h>
#include <Core/Vector.h>
#include <Core/Service.h>
#include <Core/Function.h>

DeclareHandle(Entity)
DeclareType(Entity)

typedef void(*EntityHandler)(Entity entity);
typedef bool(*EntityBoolHandler)(Entity entity);
typedef void(*ComponentHandler)(Entity entity, Type componentType);
typedef void(*PropertyChangedHandler)(Entity entity, Type componentType, Property property);

DeclareEvent(PropertyChanged, PropertyChangedHandler)
DeclareEvent(ComponentAdded, ComponentHandler)
DeclareEvent(ComponentRemoved, ComponentHandler)
DeclareEvent(ComponentChanged, ComponentHandler)

DeclareService(Entity)

Entity GetNextEntityThat(Entity currentInList, EntityBoolHandler conditionFunc);

bool RegisterComponent(Type componentType, EntityBoolHandler addFunc, EntityBoolHandler removeFunc, EntityBoolHandler hasFunc);
bool UnregisterComponent(Type componentType);

bool AddComponent(Entity entity, Type componentType);
bool RemoveComponent(Entity entity, Type componentType);
bool HasComponent(Entity entity, Type componentType);
bool IsComponent(Type componentType);

Index GetExtensions(Type componentType);
Type GetExtension(Type componentType, Index index);
Index AddExtension(Type componentType, Type extensionType);
bool RemoveExtension(Type componentType, Type extensionType);
bool RemoveExtensionByIndex(Type componentType, Index index);

Index GetDependencies(Type componentType);
Type GetDependency(Type componentType, Index index);
Index AddDependency(Type componentType, Type dependency);
bool RemoveDependency(Type componentType, Type dependency);
bool RemoveDependencyByIndex(Type componentType, Index index);

void SetComponentAbstract(Type type, bool value);
bool IsComponentAbstract(Type type);

#define InvalidIndex 0xffffffff
#define ComponentsPerPage 1024

#define DeclareComponent(TYPENAME) \
    DeclareType(TYPENAME)\
    Entity Create ## TYPENAME(Entity parent, StringRef name);\
    bool Add ## TYPENAME (Entity entity);\
    bool Remove ## TYPENAME (Entity entity); \
    bool Has ## TYPENAME (Entity entity); \
    struct TYPENAME * Get ## TYPENAME ## ByIndex (Index index);\
    struct TYPENAME * Get ## TYPENAME (Entity entity);\
    Index GetNum ## TYPENAME (); \
    Entity Get ## TYPENAME ## Entity(Index index);\
    u64 Get ## TYPENAME ## Index(Entity entity);\
    DeclareEvent(TYPENAME ## Added, EntityHandler) \
    DeclareEvent(TYPENAME ## Removed, EntityHandler) \
    DeclareEvent(TYPENAME ## Changed, EntityHandler)

#define DefineComponent(TYPENAME) \
    DeclareType(TYPENAME)\
    DefineEvent(TYPENAME ## Added, EntityHandler) \
    DefineEvent(TYPENAME ## Removed, EntityHandler) \
    DefineEvent(TYPENAME ## Changed, EntityHandler) \
    struct _data_ ## TYPENAME {\
        TYPENAME data;\
        Entity entity;\
    };\
    u32 _num_ ## TYPENAME = 0;\
    Pool<_data_ ## TYPENAME> TYPENAME ## _data; \
    Pool<Index> TYPENAME ## _component_indices;\
    Vector<Type> TYPENAME ## _dependencies;\
    Vector<Type> TYPENAME ## _extensions;\
    Index GetNum ## TYPENAME () {\
        return _num_ ## TYPENAME;\
    } \
    Entity Get ## TYPENAME ## Entity(Index index) {\
        Assert(TYPENAME ## _data.IsValid(index));\
        return TYPENAME ## _data[index].entity;\
    }\
    bool Has ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        auto entityIndex = GetHandleIndex(entity);\
        return TYPENAME ## _component_indices.IsValid(entityIndex); \
    } \
    struct TYPENAME * Get ## TYPENAME ## ByIndex (Index index) {\
        Assert(TYPENAME ## _data.IsValid(index));\
        return &TYPENAME ## _data[index].data; \
    }\
    bool Add ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) {\
            size_t index = _num_ ## TYPENAME;\
            Assert(!TYPENAME ## _data.IsValid(index));\
            _num_ ## TYPENAME++;\
            Assert(TYPENAME ## _data.Insert(index));\
            _data_ ## TYPENAME * data = &TYPENAME ## _data[index];\
            data->entity = entity;\
            Index entityIndex = GetHandleIndex(entity); \
            Assert(TYPENAME ## _component_indices.Insert(entityIndex)); \
            TYPENAME ## _component_indices[entityIndex] = index; \
            for(Index i = 0; i < GetDependencies(TypeOf_ ## TYPENAME ()); ++i) AddComponent(entity, GetDependency(TypeOf_ ## TYPENAME (), i));\
            FireEvent(TYPENAME ## Added, entity);\
            FireEvent(ComponentAdded, entity, TypeOf_ ## TYPENAME ());\
            for(Index i = 0; i < GetExtensions(TypeOf_ ## TYPENAME ()); ++i) AddComponent(entity, GetExtension(TypeOf_ ## TYPENAME (), i));\
            return true;\
        }\
        return false;\
    }\
    u64 Get ## TYPENAME ## Index(Entity entity) {\
        return TYPENAME ## _component_indices[GetHandleIndex(entity)];\
    }\
    struct TYPENAME * Get ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        Index entityIndex = GetHandleIndex(entity); \
        size_t index = TYPENAME ## _component_indices[entityIndex];\
        return &TYPENAME ## _data[index].data; \
    }\
    Entity Create ## TYPENAME (Entity parent, StringRef name) { \
        auto entity  = CreateEntityFromName(parent, name);\
        if(IsEntityValid(entity)) Add ## TYPENAME (entity);\
        return entity;\
    }\
    bool Remove ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        if(Has ## TYPENAME (entity)) {\
            for(Index i = 0; i < GetExtensions(TypeOf_ ## TYPENAME ()); ++i) RemoveComponent(entity, GetExtension(TypeOf_ ## TYPENAME (), i));\
            FireEvent(TYPENAME ## Removed, entity);\
            FireEvent(ComponentRemoved, entity, TypeOf_ ## TYPENAME ());\
            if(Has ## TYPENAME (entity)) {\
                Index deletionEntityIndex = GetHandleIndex(entity); \
                auto deletionIndex = TYPENAME ## _component_indices[deletionEntityIndex]; \
                auto deletionData = &TYPENAME ## _data[deletionIndex];\
                auto lastIndex = _num_ ## TYPENAME - 1;\
                auto lastData = &TYPENAME ## _data[lastIndex];\
                Index lastEntityIndex = GetHandleIndex(lastData->entity); \
                if(deletionData != lastData) {\
                    std::swap(*lastData, *deletionData);\
                    TYPENAME ## _component_indices[lastEntityIndex] = deletionIndex;\
                }\
                _num_ ## TYPENAME--; \
                TYPENAME ## _component_indices.Remove(deletionEntityIndex); \
                TYPENAME ## _data.Remove(lastIndex);\
            }\
            return true;\
        }\
        return false;\
    }\
    DefineType(TYPENAME)\
        RegisterComponent(type, &Add ## TYPENAME, &Remove ## TYPENAME, &Has ## TYPENAME);

#define ExtensionOf(BASENAME) \
    AddExtension(TypeOf_ ## BASENAME (), type);

#define Abstract() \
    SetComponentAbstract(type, true);
    
#define Dependency(DEPENDENCY) AddDependency(type, TypeOf_ ## DEPENDENCY ());

#define EndComponent() EndType()

#define SetAndNotify(TYPENAME, PROPERTYTYPE, PROPERTYNAME, EVENT, NEWVALUE) \
    auto data = Get ## TYPENAME (entity); \
    auto old = data-> PROPERTYNAME; \
    data-> PROPERTYNAME = NEWVALUE; \
    FireEvent(EVENT, entity, ApiConvert<PROPERTYTYPE>(&old), NEWVALUE); \
    FireEvent(TYPENAME ## Changed, entity); \
    FireEvent(ComponentChanged, entity, TypeOf_ ## TYPENAME ())

#define DeclareComponentProperty(TYPENAME, PROPERTYTYPE, PROPERTYNAME) \
    PROPERTYTYPE Get ## PROPERTYNAME (Entity entity); \
    void Set ##  PROPERTYNAME (Entity entity, PROPERTYTYPE value);\
    void Copy ## PROPERTYNAME(Entity src, Entity dst);

#define DeclareComponentPropertyReactive(TYPENAME, PROPERTYTYPE, PROPERTYNAME) \
    DeclareComponentProperty(TYPENAME, PROPERTYTYPE, PROPERTYNAME)\
    typedef void(*PROPERTYNAME ## ChangedHandler)(Entity entity, PROPERTYTYPE oldValue, PROPERTYTYPE newValue); \
    DeclareEvent(PROPERTYNAME ## Changed, PROPERTYNAME ## ChangedHandler)

#define DefineComponentProperty(TYPENAME, PROPERTYTYPE, PROPERTYNAME) \
    PROPERTYTYPE Get ## PROPERTYNAME (Entity entity) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        return ApiConvert<PROPERTYTYPE>(&Get ## TYPENAME (entity)->PROPERTYNAME); \
    } \
    void Set ##  PROPERTYNAME (Entity entity, PROPERTYTYPE value) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        auto data = Get ## TYPENAME (entity); \
        data-> PROPERTYNAME = value; \
    }\
    void Copy ## PROPERTYNAME(Entity src, Entity dst) { Set ## PROPERTYNAME (dst, Get ## PROPERTYNAME (src)); }

#define DefineComponentPropertyReactive(TYPENAME, PROPERTYTYPE, PROPERTYNAME) \
    DefineEvent(PROPERTYNAME ## Changed, PROPERTYNAME ## ChangedHandler) \
    PROPERTYTYPE Get ## PROPERTYNAME (Entity entity) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        return ApiConvert<PROPERTYTYPE>(&Get ## TYPENAME (entity)->PROPERTYNAME); \
    } \
    void Set ##  PROPERTYNAME (Entity entity, PROPERTYTYPE value) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        SetAndNotify(TYPENAME, PROPERTYTYPE, PROPERTYNAME, PROPERTYNAME ## Changed, value); \
    }\
    void Copy ## PROPERTYNAME(Entity src, Entity dst) { Set ## PROPERTYNAME (dst, Get ## PROPERTYNAME (src)); }

#define for_entity(VARNAME, COMPONENTTYPE) \
        auto num ## COMPONENTTYPE ## VARNAME = GetNum ## COMPONENTTYPE();\
        Entity VARNAME = 0;\
        for(auto COMPONENTTYPE_i = 0; (COMPONENTTYPE_i < num ## COMPONENTTYPE ## VARNAME) && (VARNAME = Get ## COMPONENTTYPE ## Entity(COMPONENTTYPE_i)); ++COMPONENTTYPE_i)

#include <Core/Hierarchy.h>

#endif
