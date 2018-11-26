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

struct ComponentPageElement {
    Entity Entity;
    u64 Padding;
    char Data[1];
};

u32 GetNumComponentPages(Entity component);
char *GetComponentPage(Entity component, u32 index, u32 *elementStride);

void __InitializeComponent();
void __PreInitialize();

#define for_entity(ENTITYVAR, DATAVAR, COMPONENTTYPE, CONTENTS) \
    {\
        auto __component ## ENTITYVAR = ComponentOf_ ## COMPONENTTYPE ();\
        auto __numPages ## ENTITYVAR = GetNumComponentPages(__component ## ENTITYVAR);\
        for(auto __pageIndex ## ENTITYVAR = 0; __pageIndex ## ENTITYVAR < __numPages ## ENTITYVAR; ++__pageIndex ## ENTITYVAR) {\
            u32 __elementStride ## ENTITYVAR;\
            auto __pageData ## ENTITYVAR = GetComponentPage(__component ## ENTITYVAR, __pageIndex ## ENTITYVAR, &__elementStride ## ENTITYVAR);\
            for(auto __elementIndex ## ENTITYVAR = 0; __elementIndex ## ENTITYVAR < PoolPageElements; ++__elementIndex ## ENTITYVAR) {\
                auto __element ## ENTITYVAR = &__pageData ## ENTITYVAR [__elementIndex ## ENTITYVAR * __elementStride ## ENTITYVAR];\
                Entity ENTITYVAR = *(Entity*)__element ## ENTITYVAR;\
                if(!IsEntityValid(ENTITYVAR)) continue;\
                __element ## ENTITYVAR += sizeof(Entity) * 2;\
                COMPONENTTYPE * DATAVAR = (COMPONENTTYPE*)__element ## ENTITYVAR;\
                CONTENTS\
            }\
        }\
    } do {}while(false)

#define for_entity_dynamic(ENTITYVAR, COMPONENTTYPE, CONTENTS) \
    {\
        auto __component ## ENTITYVAR = COMPONENTTYPE;\
        auto __numPages ## ENTITYVAR = GetNumComponentPages(__component ## ENTITYVAR);\
        for(auto __pageIndex ## ENTITYVAR = 0; __pageIndex ## ENTITYVAR < __numPages ## ENTITYVAR; ++__pageIndex ## ENTITYVAR) {\
            u32 __elementStride ## ENTITYVAR;\
            auto __pageData ## ENTITYVAR = GetComponentPage(__component ## ENTITYVAR, __pageIndex ## ENTITYVAR, &__elementStride ## ENTITYVAR);\
            for(auto __elementIndex ## ENTITYVAR = 0; __elementIndex ## ENTITYVAR < PoolPageElements; ++__elementIndex ## ENTITYVAR) {\
                auto __element ## ENTITYVAR = &__pageData ## ENTITYVAR [__elementIndex ## ENTITYVAR * __elementStride ## ENTITYVAR];\
                Entity ENTITYVAR = *(Entity*)__element ## ENTITYVAR;\
                if(!IsEntityValid(ENTITYVAR)) continue;\
                __element ## ENTITYVAR += sizeof(Entity) * 2;\
                CONTENTS\
            }\
        }\
    } do {}while(false)

#define for_entity_parallel(ENTITYVAR, DATAVAR, COMPONENTTYPE, CONTENTS) \
    {\
        auto __component ## ENTITYVAR = ComponentOf_ ## COMPONENTTYPE ();\
        auto __numPages ## ENTITYVAR = GetNumComponentPages(__component ## ENTITYVAR);\
        auto threadnum = omp_get_thread_num();\
        auto numthreads = omp_get_num_threads();\
        auto low = __numPages ## ENTITYVAR *threadnum/numthreads;\
        auto high = __numPages ## ENTITYVAR *(threadnum+1)/numthreads;\
        for (auto __pageIndex=low; __pageIndex<high; __pageIndex++) {\
            u32 __elementStride ## ENTITYVAR;\
            auto __pageData ## ENTITYVAR = GetComponentPage(__component ## ENTITYVAR, __pageIndex, &__elementStride ## ENTITYVAR);\
            for(auto __elementIndex ## ENTITYVAR = 0; __elementIndex ## ENTITYVAR < PoolPageElements; ++__elementIndex ## ENTITYVAR) {\
                auto __element ## ENTITYVAR = &__pageData ## ENTITYVAR [__elementIndex ## ENTITYVAR * __elementStride ## ENTITYVAR];\
                Entity ENTITYVAR = *(Entity*)__element ## ENTITYVAR;\
                if(!IsEntityValid(ENTITYVAR)) continue;\
                __element ## ENTITYVAR += sizeof(Entity) * 2;\
                COMPONENTTYPE * DATAVAR = (COMPONENTTYPE*)__element ## ENTITYVAR;\
                CONTENTS\
            }\
        }\
    }

#define for_entity_abstract(ENTITYVAR, DATAVAR, COMPONENT, CONTENTS) \
    {\
        auto __component = COMPONENT;\
        auto __numPages = GetNumComponentPages(__component);\
        for(auto __pageIndex = 0; __pageIndex < __numPages; ++__pageIndex) {\
            u32 __elementStride;\
            auto __pageData = GetComponentPage(__component, __pageIndex, &__elementStride);\
            for(auto __elementIndex = 0; __elementIndex < PoolPageElements; ++__elementIndex) {\
                auto __element = &__pageData[__elementIndex * __elementStride];\
                Entity ENTITYVAR = *(Entity*)__element;\
                __element += sizeof(Entity) * 2;\
                char * DATAVAR = __element;\
                CONTENTS\
            }\
        }\
    } do {}while(false)

#endif //PLAZA_COMPONENT_H
