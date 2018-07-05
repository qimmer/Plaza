//
// Created by Kim on 22-04-2018.
//

#ifndef PLAZA_SERVICEDEFINITION_H
#define PLAZA_SERVICEDEFINITION_H

#include <Core/Entity.h>

DeclareComponent(ServiceDefinition)
    DeclareComponentChild(ServiceDefinition, ServiceDefinitionSubscriptions)
    DeclareComponentChild(ServiceDefinition, ServiceDefinitionDependencies)

DeclareComponent(ServiceDefinitionDependency)
    DeclareComponentPropertyReactive(ServiceDefinitionDependency, Service, ServiceDefinitionDependencyService)

DeclareComponent(ServiceSubscriptionDefinition)
    DeclareComponentPropertyReactive(ServiceSubscriptionDefinition, Event, ServiceSubscriptionEvent)
    DeclareComponentPropertyReactive(ServiceSubscriptionDefinition, StringRef, ServiceSubscriptionHandlerName)

#endif //PLAZA_SERVICEDEFINITION_H
