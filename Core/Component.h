//
// Created by Kim on 03/06/2018.
//

#ifndef PLAZA_COMPONENT_H
#define PLAZA_COMPONENT_H

#include <Core/NativeUtils.h>
#include <omp.h>

#define InvalidIndex 0xFFFFFFFF
#define PoolPageElements 256

Function(AddComponent, bool, Entity entity, Entity componentType)
Function(RemoveComponent, bool, Entity entity, Entity componentType)
Function(HasComponent, bool, Entity entity, Entity componentType)

Function(GetComponentMax, u32, Entity component);
Function(GetComponentEntity, Entity, Entity component, u32 index);
Function(GetComponentIndex, u32, Entity entity, Entity component);

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

    Event(EntityComponentAdded, Entity entity)
    Event(EntityComponentRemoved, Entity entity)

u32 GetComponentTypeIndexByIndex(u32 entityIndex);

char * GetComponentBytes(Entity component, u32 index);

char * GetComponentData(u32 componentTypeIndex, Entity entity);

bool IsEntityValid(Entity entity);

struct ComponentPageElement {
    Entity Entity;
    u64 Padding;
    char Data[1];
};

u32 GetNumComponentPages(Entity component);
char *GetComponentPage(Entity component, u32 index, u32 *elementStride);

void __InitializeComponent();
void __PreInitialize();

static inline Entity GetNextEntity(Entity *index, Entity component, void **data) {
    auto numPages = GetNumComponentPages(component);

    auto elementIndex = *index - ((*index / PoolPageElements) * PoolPageElements);

    for(auto pageIndex = *index / PoolPageElements; pageIndex < numPages; ++pageIndex) {
        u32 stride;
        auto pageData = GetComponentPage(component, pageIndex, &stride);

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

#define for_entity(ENTITYVAR, DATAVAR, COMPONENTTYPE) \
    COMPONENTTYPE *DATAVAR;\
    for(Entity __i = 0, __component = ComponentOf_ ## COMPONENTTYPE(), ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR); ENTITYVAR; ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR))

#define for_entity_abstract(ENTITYVAR, DATAVAR, COMPONENTTYPE) \
    char *DATAVAR;\
    for(Entity __i = 0, __component = COMPONENTTYPE, ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR); ENTITYVAR; ENTITYVAR = GetNextEntity(&__i, __component, (void**)&DATAVAR))


#define for_entity_parallel(ENTITYVAR, DATAVAR, COMPONENTTYPE, CONTENTS) \
    {\
        auto __component = ComponentOf_ ## COMPONENTTYPE ();\
        auto __numPages = GetNumComponentPages(__component);\
        auto threadnum = omp_get_thread_num();\
        auto numthreads = omp_get_num_threads();\
        auto low = __numPages *threadnum/numthreads;\
        auto high = __numPages *(threadnum+1)/numthreads;\
        for (auto __pageIndex=low; __pageIndex<high; __pageIndex++) {\
            u32 __elementStride;\
            auto __pageData = GetComponentPage(__component, __pageIndex, &__elementStride);\
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
