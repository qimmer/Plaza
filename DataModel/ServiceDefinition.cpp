//
// Created by Kim on 22-04-2018.
//

#include <Core/String.h>
#include <Core/Service.h>
#include "ServiceDefinition.h"

struct ServiceDefinition {
    Entity ServiceDefinitionSubscriptions;
};

struct ServiceSubscriptionDefinition {
    Event ServiceSubscriptionEvent;
    String ServiceSubscriptionHandlerName;
};

struct ServiceDefinitionDependency {
    Service ServiceDefinitionDependencyService;
};


DefineComponent(ServiceDefinition)
EndComponent()
DefineComponentChild(ServiceDefinition, Hierarchy, ServiceDefinitionSubscriptions)

DefineComponent(ServiceSubscriptionDefinition)
    DefineProperty(Event, ServiceSubscriptionEvent)
    DefineProperty(StringRef, ServiceSubscriptionHandlerName)
EndComponent()
DefineComponentPropertyReactive(ServiceSubscriptionDefinition, Event, ServiceSubscriptionEvent)
DefineComponentPropertyReactive(ServiceSubscriptionDefinition, StringRef, ServiceSubscriptionHandlerName)

DefineComponent(ServiceDefinitionDependency)
EndComponent()
    DefineComponentPropertyReactive(ServiceDefinitionDependency, Service, ServiceDefinitionDependencyService)