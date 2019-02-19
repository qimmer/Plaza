//
// Created by Kim on 02-08-2018.
//

#ifndef PLAZA_JSON_NATIVEUTILS_H
#define PLAZA_JSON_NATIVEUTILS_H

#include <Json/JsonPersistance.h>
#include <Core/Instance.h>

#define ModuleData(...) \
    DeserializeJsonFromString(module, module, #__VA_ARGS__ );

#define ComponentTemplate(...) \
    AddComponent(component, ComponentOf_TemplatedComponent());\
    AddComponent(GetComponentTemplate(component), component);\
    for_children(base, Bases, component) {\
        AddComponent(GetComponentTemplate(component), GetBaseComponent(base));\
    }\
    DeserializeJsonFromString(GetComponentTemplate(component), GetComponentTemplate(component), #__VA_ARGS__ );

#define Define(TYPE, NAME, ...) \
    DeserializeJsonFromString(TYPE ## Of_ ## NAME (), TYPE ## Of_ ## NAME (), #__VA_ARGS__ );

#endif //PLAZA_NATIVEUTILS_H
