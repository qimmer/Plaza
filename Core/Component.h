//
// Created by Kim on 03/06/2018.
//

#ifndef PLAZA_COMPONENT_H
#define PLAZA_COMPONENT_H

#include <Core/Pool.h>
#include <omp.h>
#include <EASTL/fixed_map.h>

#define PoolPageElements 256

typedef eastl::fixed_vector<u16, 256> EntityComponentIndexVector;

Function(AddComponent, bool, Entity entity, Entity componentType)
Function(RemoveComponent, bool, Entity entity, Entity componentType)
Function(HasComponent, bool, Entity entity, Entity componentType)

bool IsEntityValid(Entity entity);

void __InitializeComponent();
void __PreInitialize();

Entity ComponentOf_Component();

u32 GetComponentIndex(Entity component, Entity entity);
u32 GetComponentIndexByIndex(u32 componentInfoIndex, Entity entity);
Entity GetComponentEntity(u32 componentInfoIndex, u32 componentIndex);
Pool& GetComponentPool(u32 componentInfoIndex);
const void* GetComponentInstanceData(u32 componentInfoIndex, u32 componentDataIndex);
void SetComponentInstanceData(u32 componentInfoIndex, u32 componentDataIndex, const void *data);
void SetComponentData(Entity entity, Entity component, const void *data);

inline Entity GetNextEntity(u32 *componentIndex, u32 componentTypeIndex, void *data) {
	auto& pool = GetComponentPool(componentTypeIndex);
    auto numPages = pool.GetNumPages();
	u32 stride = pool.GetBlockSize();

    auto elementIndex = *componentIndex % PoolPageElements;

    for(u32 pageIndex = *componentIndex / PoolPageElements; pageIndex < numPages; ++pageIndex) {
        auto pageData = pool.GetPage(pageIndex);

        for(; elementIndex < PoolPageElements; ++elementIndex) {
            auto element = &pageData[elementIndex * stride];
            auto entity = *(Entity*)element;

            if(IsEntityValid(entity)) {
                *componentIndex = pageIndex * PoolPageElements + elementIndex + 1;
                if(data) {
                    memcpy(data, element, pool.GetElementSize());
                }

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
    ChildArray Properties, Bases;

	u16 ComponentDataIndex;
	u16 ComponentSize;
	bool ComponentExplicitSize;

	NativePtr ComponentDefaultData;
};

Unit(Component)
    Component(Extension)
        ReferenceProperty(Entity, ExtensionComponent)
        ReferenceProperty(Entity, ExtensionExtenderComponent)
        Property(bool, ExtensionDisabled)

    Component(Base)
        ReferenceProperty(Component, BaseComponent)

    Component(Component)
        Property(u16, ComponentSize)
        Property(bool, ComponentExplicitSize)
        ArrayProperty(Property, Properties)
        ArrayProperty(Base, Bases)
        Property(NativePtr, ComponentDefaultData)

#define for_entity(ENTITYVAR, COMPONENTTYPE) \
    for(Entity __i = 0, __componentTypeIndex = (Entity)GetComponentIndexByIndex(0, COMPONENTTYPE), ENTITYVAR = GetNextEntity((u32*)&__i, (u32)__componentTypeIndex, 0); ENTITYVAR; ENTITYVAR = GetNextEntity((u32*)&__i, (u32)__componentTypeIndex, 0))

#define for_entity_data(ENTITYVAR, COMPONENTTYPE, DATADESTINATION) \
    for(Entity __i = 0, __componentTypeIndex = (Entity)GetComponentIndexByIndex(0, COMPONENTTYPE), ENTITYVAR = GetNextEntity((u32*)&__i, (u32)__componentTypeIndex, DATADESTINATION); ENTITYVAR; ENTITYVAR = GetNextEntity((u32*)&__i, (u32)__componentTypeIndex, DATADESTINATION))

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
