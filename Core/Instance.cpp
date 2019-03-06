//
// Created by Kim on 27-09-2018.
//

#include "Instance.h"
#include "Identification.h"
#include "Binding.h"
#include "Debug.h"

#define Verbose_Instance "instance"

static eastl::vector<eastl::vector<Entity>> templateInstances;

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
    if(oldTemplate) {
        auto oldTemplateIndex = GetEntityIndex(oldTemplate);

        if(templateInstances.size() > oldTemplateIndex) {
            eastl::remove(templateInstances[oldTemplateIndex].begin(), templateInstances[oldTemplateIndex].end(), entity);
        }

        for_entity(component, componentData, Component) {
            if(component == ComponentOf_Identification()
               || component == ComponentOf_Ownership()
               || component == ComponentOf_UnresolvedEntity()
               || component == ComponentOf_Instance()) {
                continue;
            }

            if(HasComponent(oldTemplate, component)) {
                for_children(property, Properties, component) {
                    auto propertyData = GetPropertyData(property);

                    if(propertyData->PropertyReadOnly) continue;

                    if(propertyData->PropertyKind == PropertyKind_Child) {
                        auto instanceChild = GetPropertyValue(property, entity).as_Entity;

                        if(instanceChild) {
                            SetInstanceTemplate(instanceChild, 0);
                        }
                    } else if (propertyData->PropertyKind == PropertyKind_Array) {
                        // Remove children determined by old template

                        Vector<Entity, 32> childrenToRemove;

                        for_children_abstract(child, property, entity) {
                            auto childTemplate = GetInstanceTemplate(child);
                            auto childTemplateOwner = GetOwner(childTemplate);
                            if (childTemplateOwner == oldTemplate) {
                                childrenToRemove.push_back(child);
                            }
                        }

                        for (auto &child : childrenToRemove) {
                            auto index = GetArrayPropertyIndex(property, entity, child);
                            RemoveArrayPropertyElement(property, entity, index);
                        }
                    } else {
                        auto templateBinding = GetBinding(newTemplate, property);

                        if(templateBinding) {
                            Unbind(entity, property);
                        }
                    }
                }
            }
        }
    }

    if(newTemplate) {
        auto newTemplateIndex = GetEntityIndex(newTemplate);

        if(templateInstances.size() <= newTemplateIndex) {
            templateInstances.resize(newTemplateIndex + 1);
        }

        templateInstances[newTemplateIndex].push_back(entity);

        for_entity(component, componentData2, Component) {
            if (component == ComponentOf_Identification()
                || component == ComponentOf_Ownership()
                || component == ComponentOf_UnresolvedEntity()
                || component == ComponentOf_Instance()) {
                continue;
            }

            if (HasComponent(newTemplate, component)) {
                AddComponent(entity, component);
            }
        }

        for_entity(component, componentData, Component) {
            if(component == ComponentOf_Identification()
               || component == ComponentOf_Ownership()
               || component == ComponentOf_UnresolvedEntity()
               || component == ComponentOf_Instance()) {
                continue;
            }

            if(HasComponent(newTemplate, component)) {
                for_children(property, Properties, component) {
                    auto propertyData = GetPropertyData(property);

                    if(propertyData->PropertyReadOnly) continue;

                    if(propertyData->PropertyKind == PropertyKind_Child) {
                        auto instanceChild = GetPropertyValue(property, entity).as_Entity;
                        auto templateChild = GetPropertyValue(property, newTemplate).as_Entity;

                        SetInstanceTemplate(instanceChild, templateChild);
                    } else if(propertyData->PropertyKind == PropertyKind_Array) {
                        for_children_abstract(templateChild, property, newTemplate) {
                            auto instanceChildIndex = AddArrayPropertyElement(property, entity);
                            auto instanceChild = GetArrayPropertyElement(property, entity, instanceChildIndex);
                            SetInstanceTemplate(instanceChild, templateChild);
                        }
                    } else {
                        auto templateBinding = GetBinding(newTemplate, property);

                        if(templateBinding && !templateBinding->BindingSourceEntity) {
                            // Template has relative binding. Imitate relative binding here and don't bind directly to template value

                            auto indirections = (Entity*)alloca(sizeof(Entity) * templateBinding->BindingIndirections.size());
                            for(auto i = 0; i < templateBinding->BindingIndirections.size(); ++i) {
                                indirections[i] = templateBinding->BindingIndirections[i].IndirectionProperty;
                            }

                            Bind(entity, property, 0, indirections, (u32)templateBinding->BindingIndirections.size());
                        } else {
                            // Template has absolute binding or no binding at all. Bind to template value

                            Bind(entity, property, newTemplate, &property, 1);
                        }
                    }
                }
            }
        }
    }
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    if(valueType == TypeOf_Entity && GetPropertyKind(property) == PropertyKind_Array) {
        auto entityIndex = GetEntityIndex(entity);

        if(templateInstances.size() > entityIndex) {
            for(auto& instance : templateInstances[entityIndex]) {
                if(newValue.as_Entity) {
                    // New child added

                    auto instanceChildIndex = AddArrayPropertyElement(property, instance);
                    auto instanceChild = GetArrayPropertyElement(property, instance, instanceChildIndex);
                    SetInstanceTemplate(instanceChild, newValue.as_Entity);
                }

                else if(oldValue.as_Entity) {
                    // Existing child removed

                    for_children_abstract(child, property, instance) {
                        auto childTemplate = GetInstanceTemplate(child);
                        if (childTemplate == oldValue.as_Entity) {
                            RemoveArrayPropertyElement(property, instance, _ichild);
                            break;
                        }
                    }
                }
            }
        }
    }
}

// If components have a template and the entity has not yet a template set, apply the component template on the entity
LocalFunction(OnComponentAdding, void, Entity component, Entity entity) {
    auto templ = GetComponentTemplate(component);
    if(IsEntityValid(templ) && !GetInstanceTemplate(entity) && entity != GetComponentTemplate(component)) {
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

    RegisterGenericPropertyChangedListener(OnPropertyChanged);
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InstanceTemplate()), OnInstanceTemplateChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdding(), OnComponentAdding, 0)
EndUnit()