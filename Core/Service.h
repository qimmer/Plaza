//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_SERVICE_H
#define PLAZA_SERVICE_H

#include <Core/Handle.h>
#include <Core/Delegate.h>

DeclareHandle(Service)

typedef void(*ServiceHandler)(Service service);
typedef void(*SettingSetter)(StringRef value);
typedef StringRef(*SettingGetter)();

DeclareEvent(ServiceStarted, ServiceHandler)
DeclareEvent(ServiceStopped, ServiceHandler)

void StartService(Service service);
void StopService(Service service);
bool IsServiceRunning(Service service);

void SetServiceFunctions(Service service, Handler startFunc, Handler shutdownFunc, BoolHandler isRunningFunc);
void SetServiceName(Service service, const char *name);
const char * GetServiceName(Service service);

void SetSetting(StringRef tag, StringRef value);
StringRef GetSetting(StringRef tag);

void AddServiceDependency(Service service, Service dependency);
void AddServiceSetting(Service service, StringRef setting, SettingGetter getter, SettingSetter setter);

#define DeclareService(SERVICE) \
    DeclareEvent(SERVICE ## Started, Handler)\
    DeclareEvent(SERVICE ## Stopped, Handler)\
    void Start ## SERVICE (); \
    void Stop ## SERVICE(); \
    bool Is ## SERVICE ## Running(); \
    Service ServiceOf_ ## SERVICE ();

#define DefineService(SERVICE) \
    DefineEvent(SERVICE ## Started, Handler)\
    DefineEvent(SERVICE ## Stopped, Handler)\
    bool SERVICE ## _is_started = false; \
    static bool ServiceStart(); \
    static bool ServiceStop(); \
    void Start ## SERVICE () { \
        if(!Is ## SERVICE ## Running() && ServiceStart()) { \
            SERVICE ## _is_started = true; \
            Log(LogChannel_Core, LogSeverity_Info, "Starting %s ...", #SERVICE);\
            FireEvent(SERVICE ## Started);\
        }\
    } \
    void Stop ## SERVICE() { \
        if(Is ## SERVICE ## Running() && ServiceStop()) { \
            SERVICE ## _is_started = false; \
            Log(LogChannel_Core, LogSeverity_Info, "Stopping %s ...", #SERVICE);\
            FireEvent(SERVICE ## Stopped);\
        }\
    } \
    bool Is ## SERVICE ## Running() { return SERVICE ## _is_started; } \
    Service ServiceOf_ ## SERVICE () { \
        static Service service = 0; \
        if(!service) { \
            service = CreateService(); \
            SetServiceName(service, #SERVICE); \
            SetServiceFunctions(service, &Start ## SERVICE, &Stop ## SERVICE, &Is ## SERVICE ## Running);

#define EndService() \
        } \
        return service; \
    }

#define ServiceDependency(DEPENDENCYSERVICE) \
    AddServiceDependency(service, ServiceOf_ ## DEPENDENCYSERVICE ());

#define ServiceSetting(SETTING, TAG) \
    AddServiceSetting(service, TAG, &Get ## SETTING, &Set ## SETTING);

#endif //PLAZA_SERVICE_H
