#include <Core/Entity.h>
#include <Core/Pool.h>
#include <Core/Debug.h>
#include <Core/Dictionary.h>
#include <Core/Function.h>
#include "Delegate.h"
#include "Service.h"
#include "String.h"

struct EntityData
{
};

struct ComponentTypeData
{
    EntityBoolHandler addFunc, removeFunc, hasFunc;
    Vector<Type> dependencies, extensions;
    bool isAbstract;
};

static Pool<ComponentTypeData> componentTypes;

DefineHandle(Entity, EntityData)

DefineType(Entity)
    DefineMethod(Entity, CreateEntity)
    DefineMethod(void, DestroyEntity, Entity entity)
    DefineMethod(bool, AddComponent, Entity entity, Type componentType)
    DefineMethod(bool, RemoveComponent, Entity entity, Type componentType)
    DefineMethod(bool, HasComponent, Entity entity, Type componentType)
    DefineMethod(bool, IsComponent, Type componentType)
EndType()

DefineService(Entity)
EndService()

DefineEvent(ComponentAdded, ComponentHandler)
DefineEvent(ComponentRemoved, ComponentHandler)
DefineEvent(ComponentChanged, ComponentHandler)
DefineEvent(PropertyChanged, PropertyChangedHandler)

void SetComponentAbstract(Type componentType, bool value) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return;
    componentTypes[index].isAbstract = value;
}

bool IsComponentAbstract(Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return false;
    return componentTypes[index].isAbstract;
}

bool RegisterComponent(Type componentType, EntityBoolHandler addFunc, EntityBoolHandler removeFunc, EntityBoolHandler hasFunc) {
    auto index = GetHandleIndex(componentType);
    if(componentTypes.IsValid(index)) return false;
    componentTypes.Insert(index);

    componentTypes[index].addFunc = addFunc;
    componentTypes[index].removeFunc = removeFunc;
    componentTypes[index].hasFunc = hasFunc;

    return true;
}

bool UnregisterComponent(Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return false;
    componentTypes.Remove(index);
    return true;
}

bool AddComponent(Entity entity, Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return false;

    return componentTypes[index].addFunc(entity);
}

bool RemoveComponent(Entity entity, Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return false;

    return componentTypes[index].removeFunc(entity);
}

bool HasComponent(Entity entity, Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return false;

    return componentTypes[index].hasFunc(entity);
}

bool IsComponent(Type componentType) {
    auto index = GetHandleIndex(componentType);
    return componentTypes.IsValid(index);
}

Index GetExtensions(Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return 0;

    return componentTypes[index].extensions.size();
}

Type GetExtension(Type componentType, Index extensionIndex) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index) || extensionIndex >= componentTypes[index].extensions.size()) return 0;

    return componentTypes[index].extensions[extensionIndex];
}

Index AddExtension(Type componentType, Type extensionType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return InvalidIndex;
    componentTypes[index].extensions.push_back(extensionType);

    for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
        if(!componentTypes[index].hasFunc(entity)) continue;

        componentTypes[GetHandleIndex(extensionType)].addFunc(entity);
    }

    return componentTypes[index].extensions.size() - 1;
}

bool RemoveExtension(Type componentType, Type extensionType) {
    for(auto i = 0; i < GetExtensions(componentType); ++i) {
        if(GetExtension(componentType, i) == extensionType) {
            return RemoveExtensionByIndex(componentType, i);
        }
    }
    return false;
}

bool RemoveExtensionByIndex(Type componentType, Index extensionIndex) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index) || extensionIndex >= componentTypes[index].extensions.size()) return false;

    for(auto entity = GetNextEntity(0); IsEntityValid(entity); entity = GetNextEntity(entity)) {
        if(!componentTypes[index].hasFunc(entity)) continue;

        componentTypes[GetHandleIndex(componentTypes[index].extensions[extensionIndex])].removeFunc(entity);
    }

    componentTypes[index].extensions.erase(componentTypes[index].extensions.begin() + extensionIndex);
    return true;
}

Index GetDependencies(Type componentType) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return 0;

    return componentTypes[index].dependencies.size();
}

Type GetDependency(Type componentType, Index dependencyIndex) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index) || dependencyIndex >= componentTypes[index].dependencies.size()) return 0;

    return componentTypes[index].dependencies[dependencyIndex];
}

Index AddDependency(Type componentType, Type dependency) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index)) return InvalidIndex;
    componentTypes[index].dependencies.push_back(dependency);
    return componentTypes[index].dependencies.size() - 1;
}

bool RemoveDependencyByIndex(Type componentType, Index dependencyIndex) {
    auto index = GetHandleIndex(componentType);
    if(!componentTypes.IsValid(index) || dependencyIndex >= componentTypes[index].dependencies.size()) return false;
    componentTypes[index].dependencies.erase(componentTypes[index].dependencies.begin() + dependencyIndex);
    return true;
}

bool RemoveDependency(Type componentType, Type dependencyType) {
    for(auto i = 0; i < GetDependencies(componentType); ++i) {
        if(GetDependency(componentType, i) == dependencyType) {
            return RemoveDependencyByIndex(componentType, i);
        }
    }
    return false;
}

Entity GetNextEntityThat(Entity currentInList, EntityBoolHandler conditionFunc) {
    currentInList = GetNextEntity(currentInList);
    while(IsEntityValid(currentInList) && !conditionFunc(currentInList)) {
        currentInList = GetNextEntity(currentInList);
    }
    return currentInList;
}

static void OnEntityDestroy(Entity entity) {
    for(auto i = 0; i < componentTypes.End(); ++i ) {
        if(componentTypes.IsValid(i)) {
            componentTypes[i].removeFunc(entity);
        }
    }
}

static bool ServiceStart() {
    SubscribeEntityDestroyed(OnEntityDestroy);
    return true;
}

static bool ServiceStop() {
    UnsubscribeEntityDestroyed(OnEntityDestroy);
    return true;
}

Dictionary<Type, Vector<ComponentHandler>> OnComponentAdded;
Dictionary<Type, Vector<ComponentHandler>> OnComponentRemoved;
Dictionary<Property, Vector<PropertyChangedHandler>> OnPropertyChanged;
