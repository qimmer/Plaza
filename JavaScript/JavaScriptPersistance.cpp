//
// Created by Kim on 15/07/2018.
//

#include <Foundation/NativeUtils.h>
#include <Json/NativeUtils.h>
#include <malloc.h>
#include "JavaScriptPersistance.h"
#include "JavaScriptContext.h"

BeginUnit(JavaScriptPersistance)
    ModuleData(
            {
                "FileTypes": [
                    { "Uuid": "FileType.JavaScript", "FileTypeExtension": ".js", "FileTypeMimeType" : "application/javascript" }
                ]
            }
    );
EndUnit()
