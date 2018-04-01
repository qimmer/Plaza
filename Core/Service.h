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

void AddServiceDependency(Service service, Service dependency);
void AddServiceSubscription(Service service, void *subscribeFunc, void *unsubscribeFunc, void *eventHandler);
void AddServiceEntity(Service service, u64 *handlePtr, void* initializeFunc, StringRef name);

#define DeclareService(SERVICE) \
    DeclareEvent(SERVICE ## Started, ServiceHandler)\
    DeclareEvent(SERVICE ## Stopped, ServiceHandler)\
    bool Is ## SERVICE ## Running(); \
    Service ServiceOf_ ## SERVICE ();

#define DefineService(SERVICE) \
    DefineEvent(SERVICE ## Started, ServiceHandler)\
    DefineEvent(SERVICE ## Stopped, ServiceHandler)\
    bool SERVICE ## _is_started = false; \
    void Start ## SERVICE () { \
        if(!Is ## SERVICE ## Running()) { \
            SERVICE ## _is_started = true; \
            Log(LogChannel_Core, LogSeverity_Info, "    Starting Service '%s' ...", #SERVICE);\
            auto service = ServiceOf_ ## SERVICE ();\
            FireEvent(SERVICE ## Started, service);\
        }\
    } \
    void Stop ## SERVICE() { \
        if(Is ## SERVICE ## Running()) { \
            SERVICE ## _is_started = false; \
            Log(LogChannel_Core, LogSeverity_Info, "    Stopping Service '%s' ...", #SERVICE);\
            auto service = ServiceOf_ ## SERVICE ();\
            FireEvent(SERVICE ## Stopped, service);\
        }\
    } \
    bool Is ## SERVICE ## Running() { return SERVICE ## _is_started; } \
    Service _InitService_ ## SERVICE();\
    Service ServiceOf_ ## SERVICE () { \
        static Service service = 0; \
        if(!service) service = _InitService_ ## SERVICE ();\
        return service;\
    }\
    Service _InitService_ ## SERVICE() { \
        auto _service = CreateService(); \
        SetServiceName(_service, #SERVICE); \
        SetServiceFunctions(_service, &Start ## SERVICE, &Stop ## SERVICE, &Is ## SERVICE ## Running);

#define EndService() \
        return _service; \
    }

#define Subscribe(EVENT, HANDLER) Unsubscribe ## EVENT (0, HANDLER); AddServiceSubscription(_service, (void*)Subscribe ## EVENT, (void*)Unsubscribe ## EVENT,(void*) HANDLER);

#define ServiceEntity(ENTITY, ENTITYINITIALIZEFUNC) AddServiceEntity(_service, &ENTITY, (void*)ENTITYINITIALIZEFUNC, #ENTITY);

#define ServiceDependency(DEPENDENCYSERVICE) \
    AddServiceDependency(_service, ServiceOf_ ## DEPENDENCYSERVICE ());

#endif //PLAZA_SERVICE_H
