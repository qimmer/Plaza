//
// Created by Kim on 15/07/2018.
//

#include <Foundation/NativeUtils.h>
#include <malloc.h>
#include "JavaScriptPersistance.h"
#include "JavaScriptContext.h"


static bool Serialize(Entity persistancePoint) {
    return true;
}

static bool Deserialize(Entity persistancePoint) {
    if(!StreamOpen(persistancePoint, StreamMode_Read)) {
        Log(persistancePoint, LogSeverity_Error, "Could not open JavaScript file '%s' for reading.", GetStreamPath(persistancePoint));
        return false;
    }

    StreamSeek(persistancePoint, StreamSeek_End);
    auto size = StreamTell(persistancePoint);
    StreamSeek(persistancePoint, 0);

    auto code = (char*)malloc(size + 1);
    StreamRead(persistancePoint, size, code);
    code[size] = '\0';
    StreamClose(persistancePoint);

    auto module = persistancePoint;
    while(IsEntityValid(GetParent(module))) module = GetParent(module);

    EvaluateJavaScript(NodeOf_JavaScriptMainContext(), GetStreamPath(persistancePoint), code);

    free(code);

    return true;
}

BeginUnit(JavaScriptPersistance)
    RegisterSerializer(JavaScriptSerializer, "application/javascript")
    RegisterFileType(".js", "application/javascript", 0)
EndUnit()
