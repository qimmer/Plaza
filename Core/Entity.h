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

#define DeclareComponent(TYPENAME) \
    DeclareType(TYPENAME)\
    Entity Create ## TYPENAME(StringRef entityPath);\
    bool Add ## TYPENAME (Entity entity);\
    bool Remove ## TYPENAME (Entity entity); \
    bool Has ## TYPENAME (Entity entity); \
    Index GetNum ## TYPENAME (); \
    Entity Get ## TYPENAME ## Entity(Index index);\
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
    Vector<_data_ ## TYPENAME> TYPENAME ## _data; \
    Pool<Index> TYPENAME ## _component_indices;\
    Vector<Type> TYPENAME ## _dependencies;\
    Vector<Type> TYPENAME ## _extensions;\
    Index GetNum ## TYPENAME () {\
        return TYPENAME ## _data.size();\
    } \
    Entity Get ## TYPENAME ## Entity(Index index) {\
        Assert(index >= 0 && index < TYPENAME ## _data.size());\
        return TYPENAME ## _data[index].entity;\
    }\
    bool Has ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        auto entityIndex = GetHandleIndex(entity);\
        return TYPENAME ## _component_indices.End() > entityIndex && TYPENAME ## _component_indices[entityIndex] != InvalidIndex; \
    } \
    static TYPENAME * Get ## TYPENAME ## ByIndex (Index index) {\
        Assert(index >= 0 && index < TYPENAME ## _data.size());\
        return &TYPENAME ## _data[index].data; \
    }\
    bool Add ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) {\
            TYPENAME ## _data.push_back(_data_ ## TYPENAME ());\
            size_t index = TYPENAME ## _data.size() - 1;\
            Index entityIndex = GetHandleIndex(entity); \
            auto oldSize = TYPENAME ## _component_indices.End(); \
            TYPENAME ## _component_indices.Insert(entityIndex); \
            TYPENAME ## _data[index].entity = entity;\
            auto newSize = TYPENAME ## _component_indices.End(); \
            for(auto i = oldSize; i < newSize; ++i) TYPENAME ## _component_indices[i] = InvalidIndex; \
            TYPENAME ## _component_indices[entityIndex] = index; \
            for(Index i = 0; i < GetDependencies(TypeOf_ ## TYPENAME ()); ++i) AddComponent(entity, GetDependency(TypeOf_ ## TYPENAME (), i));\
            FireEvent(TYPENAME ## Added, entity);\
            FireEvent(ComponentAdded, entity, TypeOf_ ## TYPENAME ());\
            for(Index i = 0; i < GetExtensions(TypeOf_ ## TYPENAME ()); ++i) AddComponent(entity, GetExtension(TypeOf_ ## TYPENAME (), i));\
            return true;\
        }\
        return false;\
    }\
    static u64 Get ## TYPENAME ## Index(Entity entity) {\
        return TYPENAME ## _component_indices[GetHandleIndex(entity)];\
    }\
    static TYPENAME * Get ## TYPENAME (Entity entity) {\
        if(!IsEntityValid(entity)) return NULL;\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        Index entityIndex = GetHandleIndex(entity); \
        return &TYPENAME ## _data[TYPENAME ## _component_indices[entityIndex]].data; \
    }\
    Entity Create ## TYPENAME (StringRef entityPath) { \
        auto entity  = CreateEntityFromPath(entityPath);\
        Add ## TYPENAME (entity);\
        return entity;\
    }\
    bool Remove ## TYPENAME (Entity entity) {\
        Assert(IsEntityValid(entity));\
        if(Has ## TYPENAME (entity)) {\
            for(Index i = 0; i < GetExtensions(TypeOf_ ## TYPENAME ()); ++i) RemoveComponent(entity, GetExtension(TypeOf_ ## TYPENAME (), i));\
            FireEvent(TYPENAME ## Removed, entity);\
            FireEvent(ComponentRemoved, entity, TypeOf_ ## TYPENAME ());\
            Index deletionEntityIndex = GetHandleIndex(entity); \
            auto deletionIndex = TYPENAME ## _component_indices[deletionEntityIndex]; \
            auto lastIndex = TYPENAME ## _data.size() - 1;\
            Index moveEntityIndex = GetHandleIndex(TYPENAME ## _data[lastIndex].entity); \
            std::swap(TYPENAME ## _data[deletionIndex], TYPENAME ## _data[lastIndex]);\
            TYPENAME ## _component_indices[moveEntityIndex] = deletionIndex;\
            TYPENAME ## _data.pop_back(); \
            TYPENAME ## _component_indices[deletionEntityIndex] = InvalidIndex; \
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
    FireEvent(EVENT, entity, ApiConvert<PROPERTYTYPE>(old), NEWVALUE); \
    FireEvent(TYPENAME ## Changed, entity); \
    FireEvent(ComponentChanged, entity, TypeOf_ ## TYPENAME ())

#define DeclareComponentProperty(TYPENAME, PROPERTYTYPE, PROPERTYNAME) \
    PROPERTYTYPE Get ## PROPERTYNAME (Entity entity); \
    void Set ##  PROPERTYNAME (Entity entity, PROPERTYTYPE value); \
    typedef void(*PROPERTYNAME ## ChangedHandler)(Entity entity, PROPERTYTYPE oldValue, PROPERTYTYPE newValue); \
    DeclareEvent(PROPERTYNAME ## Changed, PROPERTYNAME ## ChangedHandler)

#define DefineComponentProperty(TYPENAME, PROPERTYTYPE, PROPERTYNAME) \
    DefineEvent(PROPERTYNAME ## Changed, PROPERTYNAME ## ChangedHandler) \
    PROPERTYTYPE Get ## PROPERTYNAME (Entity entity) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        return ApiConvert<PROPERTYTYPE>(Get ## TYPENAME (entity)->PROPERTYNAME); \
    } \
    void Set ##  PROPERTYNAME (Entity entity, PROPERTYTYPE value) { \
        Assert(IsEntityValid(entity));\
        if(!Has ## TYPENAME (entity)) Add ## TYPENAME (entity); \
        SetAndNotify(TYPENAME, PROPERTYTYPE, PROPERTYNAME, PROPERTYNAME ## Changed, value); \
    }

#define for_entity(VARNAME, CONDITIONFUNC) for(auto VARNAME = GetNextEntityThat(0, CONDITIONFUNC); IsEntityValid(VARNAME); VARNAME = GetNextEntityThat(VARNAME, CONDITIONFUNC))

#include <Core/Hierarchy.h>

#endif
