#ifndef PLAZA_DELEGATE_H
#define PLAZA_DELEGATE_H

#include <Core/Vector.h>
#include "Types.h"
#include "Handle.h"
#include <Core/Pool.h>

typedef u64 Entity;

#define FireEvent(EVENTNAME, ENTITY, ...) \
    if(std::is_same<decltype(ENTITY), Entity>::value) {\
        auto __ ## EVENTNAME ## entity = (ENTITY);\
        auto __ ## EVENTNAME ## entity_index = GetHandleIndex(__ ## EVENTNAME ## entity) + 1;\
        _ ## EVENTNAME ## Handlers.Insert(__ ## EVENTNAME ## entity_index);\
        _ ## EVENTNAME ## Handlers.Insert(0);\
        for(auto i = 0; i < _ ## EVENTNAME ## Handlers[__ ## EVENTNAME ## entity_index].size(); ++i) { _ ## EVENTNAME ## Handlers[__ ## EVENTNAME ## entity_index][i](ENTITY, ##__VA_ARGS__); }\
        for(auto i = 0; i < _ ## EVENTNAME ## Handlers[0].size(); ++i) { _ ## EVENTNAME ## Handlers[0][i](ENTITY, ##__VA_ARGS__); }\
    } else { \
        _ ## EVENTNAME ## Handlers.Insert(0);\
        for(auto i = 0; i < _ ## EVENTNAME ## Handlers[0].size(); ++i) { _ ## EVENTNAME ## Handlers[0][i](ENTITY, ##__VA_ARGS__); }\
    } do {} while(false)


#define DeclareEvent(EVENTNAME, FUNCTIONTYPE) \
    extern Pool<Vector<FUNCTIONTYPE>> _ ## EVENTNAME ## Handlers;\
    void Subscribe ## EVENTNAME (Entity entity, FUNCTIONTYPE func); \
    void Unsubscribe ## EVENTNAME (Entity entity, FUNCTIONTYPE func); \

#define DefineEvent(EVENTNAME, FUNCTIONTYPE) \
    Pool<Vector<FUNCTIONTYPE>> _ ## EVENTNAME ## Handlers;\
    void Subscribe ## EVENTNAME (Entity entity, FUNCTIONTYPE func) { \
        auto index = GetHandleIndex(entity) + 1;\
        if(!entity) index = 0;\
        Unsubscribe ## EVENTNAME(entity, func);\
        _ ## EVENTNAME ## Handlers[index].push_back(func); \
    }\
    void Unsubscribe ## EVENTNAME (Entity entity, FUNCTIONTYPE func) { \
        auto index = GetHandleIndex(entity) + 1;\
        if(!entity) index = 0;\
        _ ## EVENTNAME ## Handlers.Insert(index);\
        for(int i = _ ## EVENTNAME ## Handlers[index].size()-1; i >= 0; --i) { \
            if(_ ## EVENTNAME ## Handlers[index][i] == func) {\
                _ ## EVENTNAME ## Handlers[index].erase(_ ## EVENTNAME ## Handlers[index].begin() + i);\
                return;\
            }\
        }\
    }

typedef void(*Handler)();
typedef bool(*BoolHandler)();

#endif