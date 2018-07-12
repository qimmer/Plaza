//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_CONTEXT_H
#define PLAZA_CONTEXT_H

#include <Core/Types.h>
#include <Core/Entity.h>

Unit(Context)
    Component(Context)
        Property(StringRef, ContextTitle)
        Property(bool, ContextFullscreen)
        Property(bool, ContextVsync)
        Property(bool, ContextGrabMouse)

float *GetKeyStates(Entity context);
float GetKeyState(Entity context, u16 key);
void SetKeyState(Entity context, u16 key, float state);

v2i GetCursorPosition(Entity context, u8 index);
void SetCursorPosition(Entity context, u8 index, v2i value);

Event(KeyStateChanged, u16 key, float oldState, float newState)
Event(CharacterPressed, char c)
Event(CursorPositionChanged, u8 index, v2i oldState, v2i newState)

#endif //PLAZA_CONTEXT_H
