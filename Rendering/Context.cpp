//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/String.h>
#include "Context.h"
#include "RenderTarget.h"


    struct Context {
        Context() : ContextSize({400, 300}), ContextFullscreen(false) {}

        float KeyStates[KEY_MAX];
        v2i CursorPositions[CURSOR_MAX];
        String ContextTitle;
        v2i ContextSize;
        bool ContextFullscreen;
    };

    DefineComponent(Context)
        Dependency(RenderTarget)
        DefineProperty(StringRef, ContextTitle)
        DefineProperty(v2i, ContextSize)
    EndComponent()

    DefineComponentProperty(Context, StringRef, ContextTitle)
    DefineComponentProperty(Context, v2i, ContextSize)
    DefineComponentProperty(Context, bool, ContextFullscreen)

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
        GetContext(context)->KeyStates[(int)key] = state;

        FireEvent(KeyStateChanged, context, key, old, state);
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
