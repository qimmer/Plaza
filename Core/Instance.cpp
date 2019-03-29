//
// Created by Kim on 27-09-2018.
//

#include "Instance.h"
#include "Identification.h"
#include "Binding.h"
#include "Debug.h"

#include <EASTL/map.h>

#define Verbose_Instance "instance"

static eastl::map<Entity, eastl::vector<Entity>> templateInstances;

static bool TryResolve(Entity unresolvedReference, UnresolvedReference *data) {
    auto entity = GetOwnership(unresolvedReference).Owner;
    auto reference = FindEntityByUuid(data->UnresolvedReferenceUuid);
    if(!IsEntityValid(entity) || !IsEntityValid(reference)) {
        return false;
    }

    auto value = MakeVariant(Entity, reference);

    SetPropertyValue(data->UnresolvedReferenceProperty, entity, value);
    data->UnresolvedReferenceUuid = NULL;

    RemoveUnresolvedReferencesByValue(entity, unresolvedReference);

    if(GetNumUnresolvedReferences(entity) == 0) {
        RemoveComponent(entity, ComponentOf_UnresolvedEntity());
    }

    return true;
}
static bool ResolveOne() {
    UnresolvedReference data;
    for_entity_data(unresolvedReference, ComponentOf_UnresolvedReference(), &data) {
        if(TryResolve(unresolvedReference, &data)) return true;
    }

    return false;
}

API_EXPORT void ResolveReferences() {
    while(ResolveOne()) {}
}

static void OnInstanceChanged(Entity entity, const Instance& oldTemplate, const Instance& newTemplate) {
    if(oldTemplate.InstanceTemplate) {
        eastl::remove(templateInstances[oldTemplate.InstanceTemplate].begin(), templateInstances[oldTemplate.InstanceTemplate].end(), entity);

        for_entity (component, ComponentOf_Component()) {

			if (component == ComponentOf_Identification()
				|| component == ComponentOf_Ownership()
				|| component == ComponentOf_UnresolvedEntity()
				|| component == ComponentOf_Instance()) {
				continue;
			}

			if (!HasComponent(oldTemplate.InstanceTemplate, component)) continue;

			RemoveComponent(entity, component);
        }
    }

    if(newTemplate.InstanceTemplate) {
        templateInstances[newTemplate.InstanceTemplate].push_back(entity);

		for_entity(component, ComponentOf_Component()) {
			if (component == ComponentOf_Identification()
				|| component == ComponentOf_Ownership()
				|| component == ComponentOf_Instance()) {
				continue;
			}

			if (!HasComponent(newTemplate.InstanceTemplate, component)) continue;

			AddComponent(entity, component);

            auto componentData = GetComponent(component);
            u32 componentInfoIndex = GetComponentIndexByIndex(0, component);
            u32 newTemplateComponentIndex = GetComponentIndexByIndex(componentInfoIndex, newTemplate.InstanceTemplate);

            auto newTemplateData = (const char*)GetComponentInstanceData(componentInfoIndex, newTemplateComponentIndex);
            auto instanceData = (char*)alloca(componentData.ComponentSize);
            memset(instanceData, 0, componentData.ComponentSize);

            for_children(property, PropertyOf_Properties(), component) {
                auto propertyData = GetProperty(property);

                if(propertyData.PropertyReadOnly) continue;

                if(propertyData.PropertyType == TypeOf_Entity) {
                    auto newTemplateChild = *(Entity*)(newTemplateData + propertyData.PropertyOffset);
                    auto newTemplateOwnership = GetOwnership(newTemplateChild);
                    if(newTemplateOwnership.Owner == newTemplate.InstanceTemplate && newTemplateOwnership.OwnerProperty == property) {
                        // Actual child
                        auto instanceChild = CreateEntity();
                        SetOwnership(instanceChild, {entity, property});
                        SetIdentification(instanceChild, {StringFormatV("%s.%s", GetIdentification(entity).Uuid, GetArrayChild(property).Name)});
                        SetInstance(instanceChild, {newTemplateChild});
                        *(Entity*)(instanceData + propertyData.PropertyOffset) = instanceChild;
                    } else {
                        // Reference
                        *(Entity*)(instanceData + propertyData.PropertyOffset) = newTemplateChild;
                    }
                } else if(propertyData.PropertyType == TypeOf_ChildArray) {
                    for_children(templateChild, property, newTemplate.InstanceTemplate) {

                        auto instanceChild = CreateEntity();
                        SetOwnership(instanceChild, {entity, property});
                        SetArrayChild(instanceChild, GetArrayChild(templateChild));
                        AddArrayPropertyElement(property, entity, instanceChild);

                        SetInstance(instanceChild, {templateChild});
                    }
                } else {
                    auto templateBinding = GetBinding(newTemplate.InstanceTemplate, property);

                    if(templateBinding) {
                        auto indirections = (Entity*)alloca(sizeof(Entity) * templateBinding->BindingIndirections.size());
                        auto indirectionArrayNames = (StringRef*)alloca(sizeof(StringRef) * templateBinding->BindingIndirections.size());
                        for(auto i = 0; i < templateBinding->BindingIndirections.size(); ++i) {
                            indirections[i] = templateBinding->BindingIndirections[i].IndirectionProperty;
                            indirectionArrayNames[i] = templateBinding->BindingIndirections[i].IndirectionArrayName;
                        }

                        Bind(entity, property, templateBinding->BindingSourceEntity, indirections, indirectionArrayNames, (u32)templateBinding->BindingIndirections.size());
                    }

                    memcpy(instanceData + propertyData.PropertyOffset, newTemplateData + propertyData.PropertyOffset, GetTypeSize(propertyData.PropertyType));
                }
            }

            u32 entityComponentIndex = GetComponentIndexByIndex(componentInfoIndex, newTemplate.InstanceTemplate);
            SetComponentInstanceData(componentInfoIndex, entityComponentIndex, instanceData);
        }
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

    RegisterSystem(OnInstanceChanged, ComponentOf_Instance());
EndUnit()