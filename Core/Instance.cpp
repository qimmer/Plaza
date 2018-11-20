//
// Created by Kim on 27-09-2018.
//

#include "Instance.h"
#include "Identification.h"
#include "Debug.h"

#define Verbose_Instance "instance"

API_EXPORT bool ResolveReferences(Entity root) {
    bool areAllResolved = true;
    Entity unresolvedReference = 0;

    for_children(unresolvedReference, UnresolvedReferences, root, {
        auto data = GetUnresolvedReferenceData(unresolvedReference);

        auto entity = data->UnresolvedReferenceEntity;
        if(IsEntityValid(entity)) {
            auto reference = FindEntityByUuid(data->UnresolvedReferenceUuid);
            if(!IsEntityValid(reference)) {
                Error(root, "Property '%s' of entity '%s' has an unresolved uuid '%s'.",
                    GetUuid(data->UnresolvedReferenceProperty),
                    GetUuid(data->UnresolvedReferenceEntity),
                    data->UnresolvedReferenceUuid);

                areAllResolved = false;
                continue;
            }

            auto value = MakeVariant(Entity, reference);

            SetPropertyValue(data->UnresolvedReferenceProperty, entity, value);
        }
    });

    RemoveComponent(root, ComponentOf_UnresolvedEntity());

    return areAllResolved;
}


static void Instantiate(Entity templateEntity, Entity destinationEntity, Entity templateRoot, Entity destinationRoot);

static bool IsInstanceOverriding(Entity instance, Entity property) {
    for_children(override, InstanceOverrides, instance, {
        if(GetInstanceOverrideProperty(override) == property) return true;
    });

    return false;
}

Entity GetInstanceReference(Entity templateEntity, Entity instanceEntity, Entity referencedEntity) {
    auto templateRoot = templateEntity;
    auto instanceRoot = instanceEntity;

    while(IsEntityValid(GetOwner(instanceEntity)) && GetInstanceTemplate(GetOwner(instanceEntity)) == GetOwner(templateEntity)) {
        instanceEntity = GetOwner(instanceEntity);
        templateEntity = GetOwner(templateEntity);
    }

    auto templatePath = CalculateEntityPath(templateEntity, false);
    auto instancePath = CalculateEntityPath(instanceEntity, false);
    auto referencePath = CalculateEntityPath(referencedEntity, false);
    auto templateRootPath = CalculateEntityPath(templateRoot, false);
    auto instanceRootPath = CalculateEntityPath(instanceRoot, false);

    if(!strstr(referencePath, templateRootPath)) {
        return referencedEntity; // Referenced entity is not part of template tree. Return existing reference
    }

    auto referenceRelativePath = &referencePath[strlen(templateRootPath)];

    char instanceReferencePath[1024];
    snprintf(instanceReferencePath, sizeof(instanceReferencePath), "%s%s", instanceRootPath, referenceRelativePath);

    return FindEntityByPath(instanceReferencePath); // Return the instance tree's copy of the reference
}

static inline void InstantiateProperty(Entity templateEntity, Entity destinationEntity, Entity templateRoot, Entity instanceRoot, StringRef templateRootUuid, StringRef instanceRootUuid, StringRef templateUuid, StringRef instanceUuid, Entity property) {
    Variant value, source, destination;

    switch (GetPropertyKind(property)) {
        case PropertyKind_Value:
            value = GetPropertyValue(property, templateEntity);
            if(GetPropertyType(property) == TypeOf_Entity && IsEntityValid(value.as_Entity)) {

                auto templateReferenceUuid = GetUuid(value.as_Entity);
                auto location = strstr(templateReferenceUuid, templateRootUuid);
                if(location) {
                    // Template child Uuid contains parent uuid. Replace parent template Uuid part with instance parent uuid and use new uuid instead
                    auto relativeUuid = templateReferenceUuid + strlen(templateRootUuid);

                    char newChildUuid[1024];
                    snprintf(newChildUuid, 1024, "%s%s", instanceRootUuid, relativeUuid);

                    templateReferenceUuid = Intern(newChildUuid);
                }

                auto ref = AddUnresolvedReferences(instanceRoot);
                SetUnresolvedReferenceUuid(ref, templateReferenceUuid);
                SetUnresolvedReferenceProperty(ref, property);
                SetUnresolvedReferenceEntity(ref, destinationEntity);
            } else {
                SetPropertyValue(property, destinationEntity, value);
            }

            break;
        case PropertyKind_Child:
            source = GetPropertyValue(property, templateEntity);
            destination = GetPropertyValue(property, destinationEntity);
            if(IsEntityValid(source.as_Entity) && IsEntityValid(destination.as_Entity)) {
                Instantiate(source.as_Entity, destination.as_Entity, templateRoot, instanceRoot);
            }
            break;
        case PropertyKind_Array:
            SetArrayPropertyCount(property, destinationEntity, GetArrayPropertyCount(property, templateEntity));

            u32 sourceCount = 0;
            u32 destinationCount = 0;
            auto sourceElements = GetArrayPropertyElements(property, templateEntity, &sourceCount);
            auto destinationElements = GetArrayPropertyElements(property, destinationEntity, &destinationCount);
            for(auto i = 0; i < sourceCount; ++i) {
                auto templateChildUuid = GetUuid(sourceElements[i]);
                auto templateUuidInChildUuid = strstr(templateChildUuid, templateUuid);
                if(templateUuidInChildUuid) {
                    // Template child Uuid contains parent uuid. Replace parent template Uuid part with instance parent uuid and use new uuid instead
                    auto relativeUuid = templateChildUuid + strlen(templateUuid);

                    char newChildUuid[1024];
                    snprintf(newChildUuid, 1024, "%s%s", instanceUuid, relativeUuid);

                    SetUuid(destinationElements[i], newChildUuid);
                }
                Instantiate(sourceElements[i], destinationElements[i], templateRoot, instanceRoot);
            }
    }
}

static void Instantiate(Entity templateEntity, Entity destinationEntity, Entity templateRoot, Entity instanceRoot) {
    if(!IsEntityValid(templateEntity)) {
        Log(0, LogSeverity_Error, "Cannot copy from invalid entity", GetDebugName(templateEntity));
        return;
    }

    if(!IsEntityValid(templateEntity)) {
        Log(0, LogSeverity_Error, "Cannot copy to invalid entity", GetDebugName(destinationEntity));
        return;
    }

    AddComponent(templateEntity, ComponentOf_Template());

    SetInstanceIgnoreChanges(destinationEntity, true);

    SetInstanceTemplate(destinationEntity, templateEntity);

    auto templateUuid = GetUuid(templateEntity);
    auto instanceUuid = GetUuid(destinationEntity);
    auto templateRootUuid = GetUuid(templateRoot);
    auto instanceRootUuid = GetUuid(instanceRoot);

    for_entity(component, data, Component, {
        if (component != ComponentOf_Ownership()
        && component != ComponentOf_Template()
        && component != ComponentOf_Instance()
        && HasComponent(templateEntity, component)) {
            AddComponent(destinationEntity, component);

            for_children(property, Properties, component, {
                if(property == PropertyOf_Uuid() || GetPropertyReadOnly(property)) continue;

                if(IsInstanceOverriding(destinationEntity, property)) continue;

                InstantiateProperty(templateEntity, destinationEntity, templateRoot, instanceRoot, templateRootUuid, instanceRootUuid, templateUuid, instanceUuid, property);
            });
        }
    });

    SetInstanceIgnoreChanges(destinationEntity, false);

    // If we finished recursive instantiation, resolve all pending references
    if(templateEntity == templateRoot) {
        ResolveReferences(destinationEntity);
    }
}

LocalFunction(OnInstanceTemplateChanged, void, Entity entity, Entity oldTemplate, Entity newTemplate) {
    if(!IsEntityValid(newTemplate)) {
        return;
    }

    Verbose(Verbose_Instance, "Instantiating template %s onto instance %s", GetUuid(newTemplate), GetUuid(entity));

    Instantiate(newTemplate, entity, newTemplate, entity);
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    if(HasComponent(newOwner, ComponentOf_Template()) && !HasComponent(entity, ComponentOf_Template())) {
        AddComponent(entity, ComponentOf_Template());
    }
}

static void OnPropertyChanged(Entity property, Entity templateEntity, Type valueType, Variant oldValue, Variant newValue) {
    if(HasComponent(templateEntity, ComponentOf_Template())) {
        auto component = GetOwner(property);
        if(component == ComponentOf_Ownership()
           || property == PropertyOf_Uuid()
           || component == ComponentOf_Template()
           || component == ComponentOf_Instance()
           || component == ComponentOf_UnresolvedEntity()
           || component == ComponentOf_UnresolvedReference()) return;

        char instanceValue[128];
        auto size = GetPropertySize(property);

        switch(GetPropertyKind(property)) {
            case PropertyKind_Value:
            {
                for_entity(instance, data, Instance, {
                    if(data->InstanceTemplate != templateEntity) continue;
                    if(IsInstanceOverriding(instance, property)) continue;

                    Verbose(Verbose_Instance, "Template %s updates %s on instance %s", GetUuid(templateEntity), GetUuid(property), GetUuid(instance));

                    SetPropertyValue(property, instance, GetPropertyValue(property, instance));
                });
                break;
            }
            case PropertyKind_Array:
            {
                auto newEntity = newValue.as_Entity;
                auto oldEntity = oldValue.as_Entity;
                // If element has been added, add one to all instances and bind template/instance relation
                if(newEntity && !oldEntity) {
                    for_entity(instance, data, Instance, {
                        if(data->InstanceTemplate != templateEntity) continue;

                        auto addedIndex = AddArrayPropertyElement(property, instance);
                        SetInstanceTemplate(GetArrayPropertyElement(property, instance, addedIndex), newEntity);
                    });
                }

                // If element has been removed, remove all instanced elements as well
                if(!newEntity && oldEntity) {
                    for_entity(instance, data, Instance, {
                        if(data->InstanceTemplate != templateEntity) continue;

                        u32 count = 0;
                        auto elements = GetArrayPropertyElements(property, instance, &count);
                        for(auto i = 0; i < count; ++i) {
                            if(GetInstanceTemplate(elements[i]) == oldEntity) {
                                RemoveArrayPropertyElement(property, instance, i);
                                --count;
                                --i;
                                break;
                            }
                        }
                    });
                }
                break;
            }
            default:
                break;
        }
    }
}

LocalFunction(OnTemplateComponentAdded, void, Entity component, Entity entity) {
    if(HasComponent(entity, ComponentOf_Template())) {
        for_entity(instance, instanceData, Instance, {
            if(instanceData->InstanceTemplate == entity) {
                AddComponent(instance, component);
            }
        });
    }
}

LocalFunction(OnTemplateComponentRemoved, void, Entity component, Entity entity) {
    if(HasComponent(entity, ComponentOf_Template())) {
        for_entity(instance, instanceData, Instance, {
            if(instanceData->InstanceTemplate == entity) {
                RemoveComponent(instance, component);
            }
        });
    }
}

BeginUnit(Instance)
    BeginComponent(Instance)
        RegisterReferenceProperty(Template, InstanceTemplate)
        RegisterArrayProperty(InstanceOverride, InstanceOverrides)
        RegisterProperty(bool, InstanceIgnoreChanges)
    EndComponent()

    BeginComponent(InstanceOverride)
        RegisterReferenceProperty(Property, InstanceOverrideProperty)
    EndComponent()

    BeginComponent(Template)
    EndComponent()

    BeginComponent(UnresolvedReference)
        RegisterReferenceProperty(Property, UnresolvedReferenceProperty)
        RegisterProperty(Entity, UnresolvedReferenceEntity)
        RegisterProperty(StringRef, UnresolvedReferenceUuid)
    EndComponent()

    BeginComponent(UnresolvedEntity)
        RegisterArrayProperty(UnresolvedReference, UnresolvedReferences)
    EndComponent()

    RegisterGenericPropertyChangedListener(OnPropertyChanged);
    RegisterSubscription(EventOf_EntityComponentAdded(), OnTemplateComponentAdded, 0)
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnTemplateComponentRemoved, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InstanceTemplate()), OnInstanceTemplateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
EndUnit()