//
// Created by Kim on 27-09-2018.
//

#include <Foundation/NativeUtils.h>
#include "Instance.h"
#include "Identification.h"
#include "Debug.h"

#define Verbose_Instance "instance"

struct Template {};

struct Instance {
    bool InstanceIgnoreChanges;
    Entity InstanceTemplate;
    Vector(InstanceOverrides, Entity, 32)
};

struct InstanceOverride {
    Entity InstanceOverrideProperty;
};

static bool IsInstanceOverriding(Entity instance, Entity property) {
    for_children(override, InstanceOverrides, instance) {
        if(GetInstanceOverrideProperty(override) == property) return true;
    }

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

API_EXPORT void Instantiate(Entity templateEntity, Entity destinationEntity) {
    char buffer[128];
    Entity source, destination;

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

    for_entity(component, data, Component) {
        if (component != ComponentOf_Ownership() && component != ComponentOf_Template() && component != ComponentOf_Instance() && HasComponent(templateEntity, component)) {
            AddComponent(destinationEntity, component);

            for_children(property, Properties, component) {
                if(property == PropertyOf_Uuid() || GetPropertyReadOnly(property)) continue;

                if(IsInstanceOverriding(destinationEntity, property)) continue;

                switch (GetPropertyKind(property)) {
                    case PropertyKind_Value:
                        GetPropertyValue(property, templateEntity, buffer);
                        if(GetPropertyType(property) == TypeOf_Entity) {
                            // If the reference points to an entity inside the template tree, translate the reference
                            // to point into the equilivant entity in the instance tree
                            *((Entity*)buffer) = GetInstanceReference(templateEntity, destination, *((Entity*)buffer));
                        }
                        SetPropertyValue(property, destinationEntity, buffer);
                        break;
                    case PropertyKind_Child:
                        GetPropertyValue(property, templateEntity, &source);
                        GetPropertyValue(property, destinationEntity, &destination);
                        if(IsEntityValid(source) && IsEntityValid(destination)) {
                            Instantiate(source, destination);
                        }
                        break;
                    case PropertyKind_Array:
                        SetArrayPropertyCount(property, destinationEntity, GetArrayPropertyCount(property, templateEntity));

                        auto sourceElements = GetArrayPropertyElements(property, templateEntity);
                        auto destinationElements = GetArrayPropertyElements(property, destinationEntity);
                        for(auto i = 0; i < GetArrayPropertyCount(property, templateEntity); ++i) {
                            Instantiate(sourceElements[i], destinationElements[i]);
                        }
                }
            }
        }
    }

    SetInstanceIgnoreChanges(destinationEntity, false);
}

LocalFunction(OnInstanceTemplateChanged, void, Entity entity, Entity oldTemplate, Entity newTemplate) {
    if(!IsEntityValid(newTemplate)) {
        return;
    }

    Verbose(Verbose_Instance, "Instantiationg template %s onto instance %s", GetUuid(newTemplate), GetUuid(entity));

    Instantiate(newTemplate, entity);
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    if(HasComponent(newOwner, ComponentOf_Template()) && !HasComponent(entity, ComponentOf_Template())) {
        AddComponent(entity, ComponentOf_Template());
    }
}

LocalFunction(OnPropertyChanged, void, Entity property, Entity templateEntity, Variant oldValue, Variant newValue) {
    if(HasComponent(templateEntity, ComponentOf_Template())) {
        auto component = GetOwner(property);
        if(component == ComponentOf_Ownership()
           || property == PropertyOf_Uuid()
           || component == ComponentOf_Template()
           || component == ComponentOf_Instance()) return;

        char instanceValue[128];
        auto size = GetPropertySize(property);

        switch(GetPropertyKind(property)) {
            case PropertyKind_Value:
            {
                for_entity(instance, data, Instance) {
                    if(data->InstanceTemplate != templateEntity) continue;
                    if(IsInstanceOverriding(instance, property)) continue;

                    Verbose(Verbose_Instance, "Template %s updates %s on instance %s", GetUuid(templateEntity), GetUuid(property), GetUuid(instance));

                    GetPropertyValue(property, instance, instanceValue);
                    SetPropertyValue(property, instance, &newValue.data);
                }
                break;
            }
            case PropertyKind_Array:
            {
                // If element has been added, add one to all instances and bind template/instance relation
                if(newValue.as_Entity && !oldValue.as_Entity) {
                    for_entity(instance, data, Instance) {
                        if(data->InstanceTemplate != templateEntity) continue;

                        auto addedIndex = AddArrayPropertyElement(property, instance);
                        SetInstanceTemplate(GetArrayPropertyElement(property, instance, addedIndex), newValue.as_Entity);
                    }
                }

                // If element has been removed, remove all instanced elements as well
                if(!newValue.as_Entity && oldValue.as_Entity) {
                    for_entity(instance, data, Instance) {
                        if(data->InstanceTemplate != templateEntity) continue;

                        auto elements = GetArrayPropertyElements(property, instance);
                        for(auto i = 0; i < GetArrayPropertyCount(property, instance); ++i) {
                            if(GetInstanceTemplate(elements[i]) == oldValue.as_Entity) {
                                RemoveArrayPropertyElement(property, instance, i);
                                break;
                            }
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    if(property != PropertyOf_InstanceOverrides() && HasComponent(templateEntity, ComponentOf_Instance())) {
        if(!GetInstanceIgnoreChanges(templateEntity) && !IsInstanceOverriding(templateEntity, property)) {
            // Tag this property as overridden by this instance!
            SetInstanceOverrideProperty(AddInstanceOverrides(templateEntity), property);

            Verbose(Verbose_Instance, "Instance %s overrides %s", GetUuid(templateEntity), GetUuid(property));
        }
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

    RegisterSubscription(PropertyChanged, OnPropertyChanged, 0)
    RegisterSubscription(InstanceTemplateChanged, OnInstanceTemplateChanged, 0)
    RegisterSubscription(OwnerChanged, OnOwnerChanged, 0)
EndUnit()