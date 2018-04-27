//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Service.h>
#include <algorithm>
#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>
#include "Pool.h"
#include "String.h"
#include "Dictionary.h"
#include "Entity.h"

typedef void(*SubscriptionHandler)(Entity context, void *handler);

struct Subscription {
    SubscriptionHandler SubscribeHandler, UnsubscribeHandler;
    void *EventHandler;
};

struct ServiceEntity {
    Entity *handle;
    EntityHandler createHandler;
    StringRef path;
};

struct ServiceData {
    Handler startFunc, shutdownFunc;
    BoolHandler isRunningFunc;
    String name;
    Vector<Service> dependencies;
    Vector<Subscription> subscriptions;
    Vector<ServiceEntity> serviceEntities;
};

DefineHandle(Service, ServiceData)

DefineEvent(ServiceStarted, ServiceHandler)
DefineEvent(ServiceStopped, ServiceHandler)

void StartService(Service service) {
    Assert(IsServiceValid(service));

    auto data = ServiceAt(service);
    for(auto dependency : data->dependencies) {
        StartService(dependency);
    }

    if(!IsServiceRunning(service)) {
        auto data = ServiceAt(service);

        for(auto& subscription : data->subscriptions) {
            subscription.SubscribeHandler(0, subscription.EventHandler);
        }

        for(auto& serviceEntity : data->serviceEntities) {
            *serviceEntity.handle = CreateEntityFromPath(serviceEntity.path);
            if(serviceEntity.createHandler) serviceEntity.createHandler(*serviceEntity.handle);
        }

        data->startFunc();
    }
}

void StopService(Service service) {
    if(IsServiceRunning(service)) {
        auto data = ServiceAt(service);

        for(auto dependee = GetNextService(0); IsServiceValid(dependee); dependee = GetNextService(dependee)) {
            auto& dependencies = ServiceAt(dependee)->dependencies;
            if(std::find(dependencies.begin(), dependencies.end(), service) != dependencies.end()) {
                StopService(dependee);
            }
        }

        data->shutdownFunc();

        for(auto& serviceEntity : data->serviceEntities) {
            if(IsEntityValid(*serviceEntity.handle)) DestroyEntity(*serviceEntity.handle);
        }

        for(auto& subscription : data->subscriptions) {
            subscription.UnsubscribeHandler(0, subscription.EventHandler);
        }
    }
}

bool IsServiceRunning(Service service) {
    auto data = ServiceAt(service);
    return data->isRunningFunc();
}

void SetServiceFunctions(Service service, Handler startFunc, Handler shutdownFunc, BoolHandler isRunningFunc) {
    auto data = ServiceAt(service);
    data->startFunc = startFunc;
    data->shutdownFunc = shutdownFunc;
    data->isRunningFunc = isRunningFunc;
}

void SetServiceName(Service service, const char *name) {
    auto data = ServiceAt(service);
    data->name = name;
}

const char *GetServiceName(Service service) {
    auto data = ServiceAt(service);
    return data->name.c_str();
}

void AddServiceDependency(Service service, Service dependency) {
    ServiceAt(service)->dependencies.push_back(dependency);
}

void AddServiceSubscription(Service service, void *subscribeFunc, void *unsubscribeFunc, void *eventHandler) {
    ServiceAt(service)->subscriptions.push_back({
        (SubscriptionHandler)subscribeFunc,
        (SubscriptionHandler)unsubscribeFunc,
        eventHandler
    });
}

void AddServiceEntity(Service service, Entity *handle, StringRef path, EntityHandler createFunc) {
    ServiceAt(service)->serviceEntities.push_back({
        handle,
        createFunc,
        path
    });
}
