//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/String.h>
#include "Context.h"
#include "RenderTarget.h"
#include <Logic/State.h>
#include <Input/InputState.h>
#include <cfloat>
#include <math.h>
#include <Input/Key.h>


struct Context {
        float KeyStates[KEY_MAX];
        v2i CursorPositions[CURSOR_MAX];
        String ContextTitle;
        bool ContextFullscreen, ContextVsync;
    };

    DefineComponent(Context)
        Dependency(RenderTarget)
        DefinePropertyReactive(StringRef, ContextTitle)
    EndComponent()

    DefineComponentPropertyReactive(Context, StringRef, ContextTitle)
    DefineComponentPropertyReactive(Context, bool, ContextFullscreen)
    DefineComponentPropertyReactive(Context, bool, ContextVsync)

    DefineEvent(KeyStateChanged, KeyHandler)
    DefineEvent(CursorPositionChanged, CursorHandler)
    DefineEvent(ContextClosing, EntityHandler)
    DefineEvent(CharacterPressed, CharHandler)

    float GetKeyState(Entity context, u16 key) {
        Assert(key < KEY_MAX);
        return GetContext(context)->KeyStates[(int)key];
    }

    void SetKeyState(Entity context, u16 key, float state) {
        Assert(key < KEY_MAX);

        auto old = GetContext(context)->KeyStates[(int)key];

        if(fabsf(state - old) < FLT_EPSILON) {
            return;
        }

        GetContext(context)->KeyStates[(int)key] = state;

        FireEvent(KeyStateChanged, context, key, old, state);

        for_entity(entity, InputState) {
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
                value *= GetInputStateStateScale(entity);
                SetStateValue(entity, value);
            }
        }
    }

    v2i GetCursorPosition(Entity context, u8 index) {
        Assert(index < CURSOR_MAX);
        return GetContext(context)->CursorPositions[index];
    }

    void SetCursorPosition(Entity context, u8 index, v2i value) {
        Assert(index < CURSOR_MAX);

        auto old = GetContext(context)->CursorPositions[index];
        GetContext(context)->CursorPositions[index] = value;

        FireEvent(CursorPositionChanged, context, index, old, value);
    }

float *GetKeyStates(Entity context) {
    return GetContext(context)->KeyStates;
}
