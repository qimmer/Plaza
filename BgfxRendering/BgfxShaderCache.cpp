//
// Created by Kim on 21-08-2018.
//

#include <Foundation/Stream.h>
#include <Foundation/Invalidation.h>
#include <Core/Debug.h>
#include "BgfxShaderCache.h"
#include "BgfxResource.h"

#include <bgfx/bgfx.h>

struct BgfxBinaryProgram {
};

LocalFunction(OnBgfxBinaryProgramRemoved, void, Entity entity) {
    bgfx::ProgramHandle handle = { GetBgfxResourceHandle(entity) };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }
}
LocalFunction(OnValidation, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxBinaryProgram())) {
        // Eventually free old buffers
        OnBgfxBinaryProgramRemoved(entity);

        if(!StreamOpen(entity, StreamMode_Read)) {
            Log(entity, LogSeverity_Error, "Could not open binary shader for reading.");
            return;
        }

        StreamSeek(entity, StreamSeek_End);
        u32 size = StreamTell(entity);

        auto buffer = malloc(size);
        StreamSeek(entity, 0);
        StreamRead(entity, size, buffer);
        StreamClose(entity);

        SetBgfxResourceHandle(entity, bgfx::createShader(bgfx::copy(buffer, size)).idx);
        free(buffer);
    }
}

BeginUnit(BgfxShaderCache)
    BeginComponent(BgfxBinaryProgram)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EntityComponentRemoved, OnBgfxBinaryProgramRemoved, ComponentOf_BgfxBinaryProgram())
    RegisterSubscription(Validate, OnValidation, 0)
EndUnit()
