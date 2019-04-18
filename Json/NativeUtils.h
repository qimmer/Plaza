//
// Created by Kim on 02-08-2018.
//

#ifndef PLAZA_JSON_NATIVEUTILS_H
#define PLAZA_JSON_NATIVEUTILS_H

#include <Json/JsonPersistance.h>
#include <Foundation/Stream.h>
#include <Core/Instance.h>

#define ModuleDataFile(PATH) \
    {\
        auto streamData = GetStream(module);\
        streamData.StreamPath = PATH;\
        SetStream(module, streamData);\
        auto ppData = GetPersistancePoint(module);\
        ppData.PersistancePointLoaded = true;\
        SetPersistancePoint(module, ppData);\
    }

#define ModuleData(...) \
    DeserializeJsonFromString(module, module, #__VA_ARGS__ );

#define PrefabJson(...) \
    DeserializeJsonFromString(prefab, prefab, #__VA_ARGS__ );

#define Define(TYPE, NAME, ...) \
    DeserializeJsonFromString(TYPE ## Of_ ## NAME (), TYPE ## Of_ ## NAME (), #__VA_ARGS__ );

#endif //PLAZA_NATIVEUTILS_H
