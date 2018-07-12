//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/String.h>
#include "Context.h"
#include "RenderTarget.h"
#include <Input/InputState.h>
#include <cfloat>
#include <math.h>
#include <Input/Key.h>


struct Context {
        float KeyStates[KEY_MAX];
        v2i CursorPositions[CURSOR_MAX];
        String ContextTitle;
        bool ContextFullscreen, ContextVsync, ContextGrabMouse;
    };

    BeginUnit(Context)
    BeginComponent(Context)
        RegisterBase(RenderTarget)
        RegisterProperty(StringRef, ContextTitle)
        RegisterProperty(bool, ContextFullscreen)
        RegisterProperty(bool, ContextVsync)
        RegisterProperty(bool, ContextGrabMouse)
    EndComponent()

    RegisterProperty(StringRef, ContextTitle)
    RegisterProperty(bool, ContextFullscreen)
    RegisterProperty(bool, ContextVsync)
    RegisterProperty(bool, ContextGrabMouse)

    RegisterEvent(KeyStateChanged)
    RegisterEvent(CursorPositionChanged)
    RegisterEvent(CharacterPressed)

API_EXPORT float GetKeyState(Entity context, u16 key) {
        Assert(key < KEY_MAX);
        return GetContext(context)->KeyStates[(int)key];
    }

API_EXPORT void SetKeyState(Entity context, u16 key, float state) {
        Assert(key < KEY_MAX);

        auto old = GetContext(context)->KeyStates[(int)key];

        if(fabsf(state - old) < FLT_EPSILON) {
            return;
        }

        GetContext(context)->KeyStates[(int)key] = state;

        FireEvent(EventOf_KeyStateChanged(), context, key, old, state);

        for_entity(entity, data, InputState) {
            auto stateKey = GetInputStateKey(entity);
            auto pmKey = GetInputStatePrimaryModifierKey(entity);
            auto smKey = GetInputStateSecondaryModifierKey(entity);
            Assert(stateKey < KEY_MAX);
            Assert(pmKey < KEY_MAX);
            Assert(smKey < KEY_MAX);

            if(stateKey == key || pmKey == key || smKey == key ) {
                auto value = GetKeyState(context, stateKey);
                if(pmKey) value *= GetKeyState(context, pmKey);
                if(smKey) value *= GetKeyState(context, smKey);
                SetInputStateValue(entity, value);
            }
        }
    }

API_EXPORT v2i GetCursorPosition(Entity context, u8 index) {
        Assert(index < CURSOR_MAX);
        return GetContext(context)->CursorPositions[index];
    }

API_EXPORT void SetCursorPosition(Entity context, u8 index, v2i value) {
        Assert(index < CURSOR_MAX);

        auto old = GetContext(context)->CursorPositions[index];
        GetContext(context)->CursorPositions[index] = value;

        FireEvent(EventOf_CursorPositionChanged(), context, index, old, value);
    }

API_EXPORT float *GetKeyStates(Entity context) {
    return GetContext(context)->KeyStates;
}
