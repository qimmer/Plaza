//
// Created by Kim on 27-07-2018.
//

#ifndef PLAZA_DEBUGMODULE_H
#define PLAZA_DEBUGMODULE_H

#include <Core/NativeUtils.h>

Module(Debug)

Unit(DebugModule)
    Component(Debug)
        ChildProperty(DebugServer, DebugServer)
        ArrayProperty(DebugSession, DebugServerSessions)

    Component(DebugServer)

    Component(DebugSession)
        ChildProperty(EntityTracker, DebugSessionChangeTracker)

    Function(DebugAddComponent, u16, Entity responseStream, StringRef path)
    Function(DebugRemoveComponent, u16, Entity responseStream, StringRef path)
    Function(DebugGetChanges, u16, Entity responseStream, StringRef path)
    Function(DebugCreateSession, u16, Entity responseStream, StringRef path)

#endif //PLAZA_DEBUGMODULE_H
