//
// Created by Kim on 03/06/2018.
//

#ifndef PLAZA_COMPONENT_H
#define PLAZA_COMPONENT_H

#include <Core/Pool.h>
#include <omp.h>

#define InvalidIndex 0xFFFFFFFF
#define PoolPageElements 256

Function(AddComponent, bool, Entity entity, Entity componentType)
Function(RemoveComponent, bool, Entity entity, Entity componentType)
Function(HasComponent, bool, Entity entity, Entity componentType)

struct ComponentTypeData {
	ComponentTypeData() {
		this->EntityComponentIndices.Count = 0;
		this->EntityComponentIndices.DynCapacity = 0;
		this->EntityComponentIndices.DynBuf = NULL;
	}

    Pool DataBuffer;
    Vector(EntityComponentIndices, u32, 1);
};

ComponentTypeData* GetComponentType(Entity component);

inline Entity _GetComponentEntity(ComponentTypeData *componentData, u32 componentIndex) {
    auto entity = *(Entity*)componentData->DataBuffer[componentIndex];
    return entity;
}

inline char* _GetComponentData(ComponentTypeData *componentData, u32 componentIndex) {
    return componentData->DataBuffer[componentIndex] + sizeof(Entity)*2;
}

inline u32 _GetComponentIndex(ComponentTypeData *componentData, u32 entityIndex) {
    auto count = componentData->EntityComponentIndices.Count;
    if (count <= entityIndex) {
        auto newCount = UpperPowerOf2(Max(entityIndex + 1, 8));
        SetVectorAmount(componentData->EntityComponentIndices, newCount);
        memset(&GetVector(componentData->EntityComponentIndices)[count], 0xff, sizeof(u32) * (newCount - count));
    }

    auto index = GetVector(componentData->EntityComponentIndices)[entityIndex];
	return index;
}

inline u32 _GetNumComponentPages(Entity component) {
    auto componentData = GetComponentType(component);
    return componentData->DataBuffer.GetNumPages();
}

inline char *_GetComponentPage(Entity component, u32 index, u32 *elementStride) {
    auto componentData = GetComponentType(component);
    *elementStride = componentData->DataBuffer.GetBlockSize();
    return componentData->DataBuffer.GetPage(index);
}

bool IsEntityValid(Entity entity);

struct ComponentPageElement {
    Entity Entity;
    u64 Padding;
    char Data[1];
};

void __InitializeComponent();
void __PreInitialize();

u32 GetComponentIndex(Entity component, Entity entity);
Entity GetComponentEntity(Entity component, u32 componentIndex);

typedef Vector<Entity, 4> EntityComponentList;
const EntityComponentList& GetEntityComponents(Entity entity);

inline Entity GetNextEntity(Entity *index, Entity component, void **data) {
    auto numPages = _GetNumComponentPages(component);

    auto elementIndex = *index - ((*index / PoolPageElements) * PoolPageElements);

    for(auto pageIndex = *index / PoolPageElements; pageIndex < numPages; ++pageIndex) {
        u32 stride;
        auto pageData = _GetComponentPage(component, (u32)pageIndex, &stride);

        for(; elementIndex < PoolPageElements; ++elementIndex) {
            auto element = &pageData[elementIndex * stride];
            auto entity = *(Entity*)element;

            if(IsEntityValid(entity)) {
                *index = pageIndex * PoolPageElements + elementIndex + 1;
                *data = element + sizeof(Entity) * 2;
                return entity;
            }
        }

        elementIndex = 0;
    }

    return 0;
}

#include <Core/Property.h>
#include <Core/Function.h>
#include <Core/NativeUtils.h>

Unit(Component)
    Component(Extension)
        ReferenceProperty(Entity, ExtensionComponent)
        ReferenceProperty(Entity, ExtensionExtenderComponent)
        Property(bool, ExtensionDisabled)

    Component(Base)
        ReferenceProperty(Component, BaseComponent)

    Component(Component)
        __PropertyCore(Component, u16, ComponentSize)
        __PropertyCore(Component, bool, ComponentExplicitSize)
        __ArrayPropertyCore(Property, Properties)
        __ArrayPropertyCore(Base, Bases)

    Event(EntityComponentAdding, Entity component, Entity entity)
    Event(EntityComponentAdded, Entity component, Entity entity)
    Event(EntityComponentRemoved, Entity component, Entity entity)

#define for_entity(ENTITYVAR, DATAVAR, COMPONENTTYPE) \
    COMPONENTTYPE *DATAVAR;\
    for(Entity __i = 0, __component = ComponentOf_ ## COMPONENTTYPE(), ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR); ENTITYVAR; ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR))

#define for_entity_abstract(ENTITYVAR, DATAVAR, COMPONENTTYPE) \
    char *DATAVAR;\
    for(Entity __i = 0, __component = COMPONENTTYPE, ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR); ENTITYVAR; ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR))


#define for_entity_parallel(ENTITYVAR, DATAVAR, COMPONENTTYPE, CONTENTS) \
    {\
        auto __component = ComponentOf_ ## COMPONENTTYPE ();\
        auto __numPages = _GetNumComponentPages(__component);\
        auto threadnum = omp_get_thread_num();\
        auto numthreads = omp_get_num_threads();\
        auto low = __numPages *threadnum/numthreads;\
        auto high = __numPages *(threadnum+1)/numthreads;\
        for (auto __pageIndex=low; __pageIndex<high; __pageIndex++) {\
            u32 __elementStride;\
            auto __pageData = _GetComponentPage(__component, __pageIndex, &__elementStride);\
            for(auto __elementIndex = 0; __elementIndex < PoolPageElements; ++__elementIndex) {\
                auto __element = &__pageData [__elementIndex * __elementStride];\
                Entity ENTITYVAR = *(Entity*)__element;\
                if(!IsEntityValid(ENTITYVAR)) continue;\
                __element += sizeof(Entity) * 2;\
                COMPONENTTYPE * DATAVAR = (COMPONENTTYPE*)__element;\
                CONTENTS\
            }\
        }\
    }


#endif //PLAZA_COMPONENT_H
