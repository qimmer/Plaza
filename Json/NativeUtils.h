//
// Created by Kim on 02-08-2018.
//

#ifndef PLAZA_JSON_NATIVEUTILS_H
#define PLAZA_JSON_NATIVEUTILS_H

#include <Json/JsonPersistance.h>

#define ModuleData(...) \
    DeserializeJsonFromString(module, module, #__VA_ARGS__ );

#endif //PLAZA_NATIVEUTILS_H
