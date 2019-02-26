//
// Created by Kim on 27-09-2018.
//

#include "Instance.h"
#include "Identification.h"
#include "Binding.h"
#include "Debug.h"

#define Verbose_Instance "instance"

static bool TryResolve(Entity unresolvedReference, UnresolvedReference *data) {
    auto entity = GetOwner(unresolvedReference);
    auto reference = FindEntityByUuid(data->UnresolvedReferenceUuid);
    if(!IsEntityValid(entity) || !IsEntityValid(reference)) {
        return false;
    }

    auto value = MakeVariant(Entity, reference);

    SetPropertyValue(data->UnresolvedReferenceProperty, entity, value);
    data->UnresolvedReferenceUuid = NULL;

    RemoveUnresolvedReferencesByValue(entity, unresolvedReference);


    u32 count = 0;
    if(GetUnresolvedReferences(entity, &count) && count == 0) {
        RemoveComponent(entity, ComponentOf_UnresolvedEntity());
    }

    return true;
}
static bool ResolveOne() {
    for_entity(unresolvedReference, data, UnresolvedReference) {
        if(TryResolve(unresolvedReference, data)) return true;
    }

    return false;
}

API_EXPORT void ResolveReferences() {
    while(ResolveOne()) {}
}

LocalFunction(OnInstanceTemplateChanged, void, Entity entity, Entity oldTemplate, Entity newTemplate) {
    for_entity(component, componentData, Component) {
        if(component == ComponentOf_Identification()
        || component == ComponentOf_Ownership()
        || component == ComponentOf_UnresolvedEntity()
        || component == ComponentOf_Instance()
        || component == ComponentOf_Bindable()) {
            continue;
        }

        if(HasComponent(newTemplate, component)) {
            AddComponent(entity, component);

            for_children(property, Properties, component) {
                auto propertyData = GetPropertyData(property);

                if(propertyData->PropertyReadOnly) continue;

                if(propertyData->PropertyKind == PropertyKind_Child) {
                    auto instanceChild = GetPropertyValue(property, entity).as_Entity;
                    auto templateChild = GetPropertyValue(property, newTemplate).as_Entity;

                    SetInstanceTemplate(instanceChild, templateChild);
                } else {

                    auto binding = GetBinding(entity, property);
                    if(!binding) {
                        binding = AddBindings(entity);
                    }

                    auto templateBinding = GetBinding(newTemplate, property);

                    if(templateBinding && !GetBindingSourceEntity(templateBinding)) {
                        // Template has relative binding. Imitate relative binding here and don't bind directly to template value
                        u32 numIndirections = 0;
                        auto templateIndirections = GetBindingIndirections(templateBinding, &numIndirections);

                        SetNumBindingIndirections(binding, numIndirections);
                        auto instanceIndirections = GetBindingIndirections(binding, &numIndirections);

                        for(auto i = 0; i < numIndirections; ++i) {
                            SetBindingIndirectionProperty(instanceIndirections[i], GetBindingIndirectionProperty(templateIndirections[i]));
                        }

                        SetBindingSourceEntity(binding, 0);
                    } else {
                        // Template has absolute binding or no binding at all. Bind to template value
                        SetNumBindingIndirections(binding, 0);

                        auto indirection = AddBindingIndirections(binding);
                        SetBindingIndirectionProperty(indirection, property);
                        SetBindingSourceEntity(binding, newTemplate);
                    }

                    SetBindingTargetProperty(binding, property);
                }
            }
        }
    }
}

// Instantiates eventual component templates when components are added
LocalFunction(OnComponentAdded, void, Entity component, Entity entity) {
    auto templ = GetComponentTemplate(component);
    if(IsEntityValid(templ) && entity != GetComponentTemplate(component)) {
        SetInstanceTemplate(entity, templ);
    }
}

BeginUnit(Instance)
    BeginComponent(TemplatedComponent)
        RegisterChildProperty(Identification, ComponentTemplate)
    EndComponent()

    BeginComponent(Instance)
        RegisterProperty(Entity, InstanceTemplate)
    EndComponent()

    BeginComponent(UnresolvedReference)
        RegisterReferenceProperty(Property, UnresolvedReferenceProperty)
        RegisterProperty(StringRef, UnresolvedReferenceUuid)
    EndComponent()

    BeginComponent(UnresolvedEntity)
        RegisterArrayProperty(UnresolvedReference, UnresolvedReferences)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InstanceTemplate()), OnInstanceTemplateChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnComponentAdded, 0)
EndUnit()