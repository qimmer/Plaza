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
    auto entity = GetOwner(unresolvedReference);
    auto reference = FindEntityByUuid(data->UnresolvedReferenceUuid);
    if(!IsEntityValid(entity) || !IsEntityValid(reference)) {
        return false;
    }

    auto value = MakeVariant(Entity, reference);

    SetPropertyValue(data->UnresolvedReferenceProperty, entity, value);
    data->UnresolvedReferenceUuid = NULL;

    RemoveUnresolvedReferencesByValue(entity, unresolvedReference);

    if(GetUnresolvedReferences(entity).size() == 0) {
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
        eastl::remove(templateInstances[oldTemplate].begin(), templateInstances[oldTemplate].end(), entity);

        for_entity (component, data, Component) {
			if (component == ComponentOf_Identification()
				|| component == ComponentOf_Ownership()
				|| component == ComponentOf_UnresolvedEntity()
				|| component == ComponentOf_Instance()) {
				continue;
			}

			if (!HasComponent(oldTemplate, component)) continue;

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

    if(newTemplate) {
        templateInstances[newTemplate].push_back(entity);

		for_entity(component, data, Component) {
			if (component == ComponentOf_Identification()
				|| component == ComponentOf_Ownership()
				|| component == ComponentOf_Instance()) {
				continue;
			}

			if (!HasComponent(newTemplate, component)) continue;

			AddComponent(entity, component);
        }

		for_entity(component, data2, Component) {
			if (component == ComponentOf_Identification()
				|| component == ComponentOf_Ownership()
				|| component == ComponentOf_Instance()) {
				continue;
			}

			if (!HasComponent(newTemplate, component)) continue;

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

                    if(templateBinding) {
                        auto indirections = (Entity*)alloca(sizeof(Entity) * templateBinding->BindingIndirections.size());
                        auto indirectionArrayNames = (StringRef*)alloca(sizeof(StringRef) * templateBinding->BindingIndirections.size());
                        for(auto i = 0; i < templateBinding->BindingIndirections.size(); ++i) {
                            indirections[i] = templateBinding->BindingIndirections[i].IndirectionProperty;
                            indirectionArrayNames[i] = templateBinding->BindingIndirections[i].IndirectionArrayName;
                        }

                        Bind(entity, property, templateBinding->BindingSourceEntity, indirections, indirectionArrayNames, (u32)templateBinding->BindingIndirections.size());
                    } else {
                        SetPropertyValue(property, entity, GetPropertyValue(property, newTemplate));
                        //Bind(entity, property, newTemplate, &property, 0, 1);
                    }
                }
            }
        }
    }
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {
    if(valueType == TypeOf_Entity && GetPropertyKind(property) == PropertyKind_Array) {
		auto it = templateInstances.find(entity);
		if (it != templateInstances.end()) {
			for (auto& instance : it->second) {
				if (newValue.as_Entity) {
					// New child added

					auto instanceChildIndex = AddArrayPropertyElement(property, instance);
					auto instanceChild = GetArrayPropertyElement(property, instance, instanceChildIndex);
					SetInstanceTemplate(instanceChild, newValue.as_Entity);
				}

				else if (oldValue.as_Entity) {
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
	//RegisterSubscription(EventOf_EntityComponentRemoved(), OnInstanceRemoved, ComponentOf_Instance())
EndUnit()