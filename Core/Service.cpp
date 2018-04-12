//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Service.h>
#include <algorithm>
#include "Pool.h"
#include "String.h"
#include "Dictionary.h"
#include "Entity.h"


struct Setting {
        SettingGetter getter;
        SettingSetter setter;
    };

    typedef void(*SubscriptionHandler)(Entity context, void *handler);

    struct Subscription {
        SubscriptionHandler SubscribeHandler, UnsubscribeHandler;
        void *EventHandler;
    };

    struct ServiceEntity {
        Entity *handlePtr;
        EntityHandler initializeFunc;
        String name;
    };

    struct ServiceData {
        Handler startFunc, shutdownFunc;
        BoolHandler isRunningFunc;
        String name;
        Vector<Service> dependencies;
        Vector<Subscription> subscriptions;
        Vector<ServiceEntity> entities;
    };

    Dictionary<Setting> SettingsTable;

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

            for(auto& serviceEntity : data->entities) {
                char entityPath[PATH_MAX];
                snprintf(entityPath, PATH_MAX, "/.%s/%s", data->name.c_str(), serviceEntity.name.c_str());

                *serviceEntity.handlePtr = CreateEntityFromPath(entityPath);
                if(serviceEntity.initializeFunc) serviceEntity.initializeFunc(*serviceEntity.handlePtr);
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

            for(auto& serviceEntity : data->entities) {
                if(IsEntityValid(*serviceEntity.handlePtr)) DestroyEntity(*serviceEntity.handlePtr);
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
    ServiceAt(service)->subscriptions.push_back({(SubscriptionHandler)subscribeFunc, (SubscriptionHandler)unsubscribeFunc, eventHandler});
}

void AddServiceEntity(Service service, u64 *handlePtr, void *initializeFunc, StringRef name) {
    ServiceAt(service)->entities.push_back({(Entity*)handlePtr, (EntityHandler)initializeFunc, name});
}
