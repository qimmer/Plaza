//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Service.h>
#include "Pool.h"
#include "String.h"
#include "Dictionary.h"


    struct Setting {
        SettingGetter getter;
        SettingSetter setter;
    };

    struct ServiceData {
        Handler startFunc, shutdownFunc;
        BoolHandler isRunningFunc;
        String name;
        Vector<String> settings;
        Vector<Service> dependencies;
    };

    Dictionary<String, Setting> SettingsTable;

    DefineHandle(Service, ServiceData)

    DefineEvent(ServiceStarted, ServiceHandler)
    DefineEvent(ServiceStopped, ServiceHandler)

    void StartService(Service service) {
        for(auto dependency : ServiceAt(service).dependencies) {
            StartService(dependency);
        }

        if(!IsServiceRunning(service)) {
            auto& data = ServiceAt(service);
            data.startFunc();
        }
    }

    void StopService(Service service) {
        if(IsServiceRunning(service)) {
            auto& data = ServiceAt(service);

            for(auto dependee = GetNextService(0); IsServiceValid(dependee); dependee = GetNextService(dependee)) {
                auto& dependencies = ServiceAt(dependee).dependencies;
                if(std::find(dependencies.begin(), dependencies.end(), service) != dependencies.end()) {
                    StopService(dependee);
                }
            }

            data.shutdownFunc();
        }
    }

    bool IsServiceRunning(Service service) {
        auto& data = ServiceAt(service);
        return data.isRunningFunc();
    }

    void SetServiceFunctions(Service service, Handler startFunc, Handler shutdownFunc, BoolHandler isRunningFunc) {
        auto& data = ServiceAt(service);
        data.startFunc = startFunc;
        data.shutdownFunc = shutdownFunc;
        data.isRunningFunc = isRunningFunc;
    }

    void SetServiceName(Service service, const char *name) {
        auto& data = ServiceAt(service);
        data.name = name;
    }

    const char *GetServiceName(Service service) {
        auto& data = ServiceAt(service);
        return data.name.c_str();
    }

    void SetSetting(StringRef setting, StringRef value) {
        auto it = SettingsTable.find(setting);
        if(it != SettingsTable.end()) {
            (*it).second.setter(value);
        }
    }

    StringRef GetSetting(StringRef setting) {
        auto it = SettingsTable.find(setting);
        if(it != SettingsTable.end()) {
            return (*it).second.getter();
        }

        return "";
    }

    void AddServiceSetting(Service service, StringRef setting, SettingGetter getter, SettingSetter setter) {
        Setting s;
        s.getter = getter;
        s.setter = setter;

        ServiceAt(service).settings.push_back(setting);
        SettingsTable[setting] = s;
    }

    void AddServiceDependency(Service service, Service dependency) {
        ServiceAt(service).dependencies.push_back(dependency);
    }
