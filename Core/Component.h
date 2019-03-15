//
// Created by Kim on 03/06/2018.
//

#ifndef PLAZA_COMPONENT_H
#define PLAZA_COMPONENT_H

#include <Core/Pool.h>
#include <omp.h>
#include <EASTL/fixed_map.h>

#define InvalidIndex 0xFFFFFFFF
#define PoolPageElements 256

typedef eastl::fixed_map<Entity, u32, 16> EntityComponentIndexMap;

Function(AddComponent, bool, Entity entity, Entity componentType)
Function(RemoveComponent, bool, Entity entity, Entity componentType)
Function(HasComponent, bool, Entity entity, Entity componentType)

bool IsEntityValid(Entity entity);

struct ComponentPageElement {
    Entity Entity;
    u64 Padding;
    char Data[1];
};

void __InitializeComponent();
void __PreInitialize();

Entity ComponentOf_Component();

u32 GetComponentIndex(Entity component, Entity entity);
Entity GetComponentEntity(Entity component, u32 componentDataIndex);
Pool& GetComponentPool(Entity component);
char* GetComponentData(Entity component, u32 componentIndex);

const EntityComponentIndexMap& GetEntityComponents(Entity entity);

inline Entity GetNextEntity(Entity *index, Entity component, void **data) {
	if (!HasComponent(component, ComponentOf_Component())) return 0;

	auto& pool = GetComponentPool(component);
    auto numPages = pool.GetNumPages();
	u32 stride = pool.GetBlockSize();

    auto elementIndex = *index - ((*index / PoolPageElements) * PoolPageElements);

    for(auto pageIndex = *index / PoolPageElements; pageIndex < numPages; ++pageIndex) {
        auto pageData = pool.GetPage((u32)pageIndex);

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

struct Extension {
	Entity ExtensionComponent, ExtensionExtenderComponent;
	bool ExtensionDisabled;
};

struct Base {
	Entity BaseComponent;
};

struct Component {
	u16 ComponentDataIndex;
	u16 ComponentSize;
	bool ComponentExplicitSize;
};

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
