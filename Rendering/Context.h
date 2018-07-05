//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_CONTEXT_H
#define PLAZA_CONTEXT_H

#include <Core/Types.h>
#include <Core/Entity.h>

DeclareComponent(Context)
DeclareComponentPropertyReactive(Context, StringRef, ContextTitle)
DeclareComponentPropertyReactive(Context, bool, ContextFullscreen)
DeclareComponentPropertyReactive(Context, bool, ContextVsync)
DeclareComponentPropertyReactive(Context, bool, ContextGrabMouse)

float *GetKeyStates(Entity context);
float GetKeyState(Entity context, u16 key);
void SetKeyState(Entity context, u16 key, float state);

v2i GetCursorPosition(Entity context, u8 index);
void SetCursorPosition(Entity context, u8 index, v2i value);

DeclareEvent(KeyStateChanged, Entity context, u16 key, float oldState, float newState)
DeclareEvent(CharacterPressed, Entity context, char c)
DeclareEvent(CursorPositionChanged, Entity context, u8 index, v2i oldState, v2i newState)

#endif //PLAZA_CONTEXT_H
