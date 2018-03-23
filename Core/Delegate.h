#ifndef PLAZA_DELEGATE_H
#define PLAZA_DELEGATE_H

#include <Core/Vector.h>
#include "Types.h"

#define FireEvent(EVENTNAME, ...) for(auto i = 0; i < _ ## EVENTNAME ## Handlers.size(); ++i) { _ ## EVENTNAME ## Handlers[i](__VA_ARGS__); } do {} while(false)

#define DeclareEvent(EVENTNAME, FUNCTIONTYPE) \
    extern Vector<FUNCTIONTYPE> _ ## EVENTNAME ## Handlers;\
    void Subscribe ## EVENTNAME (FUNCTIONTYPE func); \
    void Unsubscribe ## EVENTNAME (FUNCTIONTYPE func); \

#define DefineEvent(EVENTNAME, FUNCTIONTYPE) \
    Vector<FUNCTIONTYPE> _ ## EVENTNAME ## Handlers;\
    void Subscribe ## EVENTNAME (FUNCTIONTYPE func) { \
        _ ## EVENTNAME ## Handlers.push_back(func); \
    }\
    void Unsubscribe ## EVENTNAME (FUNCTIONTYPE func) { \
        for(int i = _ ## EVENTNAME ## Handlers.size()-1; i >= 0; --i) { \
            if(_ ## EVENTNAME ## Handlers[i] == func) {\
                _ ## EVENTNAME ## Handlers.erase(_ ## EVENTNAME ## Handlers.begin() + i);\
                return;\
            }\
        }\
    }

typedef void(*Handler)();
typedef bool(*BoolHandler)();

#endif