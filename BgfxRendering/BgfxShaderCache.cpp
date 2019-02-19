//
// Created by Kim on 21-08-2018.
//

#include <Foundation/Stream.h>
#include <Foundation/Invalidation.h>
#include <Core/Debug.h>
#include "BgfxShaderCache.h"
#include "BgfxResource.h"

#include <bgfx/bgfx.h>
#include <Rendering/ShaderCache.h>

struct BgfxBinaryProgram {
};

LocalFunction(OnBgfxBinaryProgramRemoved, void, Entity entity) {
    bgfx::ProgramHandle handle = { GetBgfxResourceHandle(entity) };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }
}

static void free_func(void* mem, void *userdata) {
    _mm_free(mem);
}

static void CompileProgram(Entity entity) {
    // Eventually free old buffers
    OnBgfxBinaryProgramRemoved(entity);

    auto binaryVertexShader = GetBinaryProgramVertexShader(entity);
    if(!StreamOpen(binaryVertexShader, StreamMode_Read)) {
        Log(binaryVertexShader, LogSeverity_Error, "Could not open binary vertex shader for reading.");
        return;
    }

    auto binaryPixelShader = GetBinaryProgramPixelShader(entity);
    if(!StreamOpen(binaryPixelShader, StreamMode_Read)) {
        StreamClose(binaryVertexShader);
        Log(binaryPixelShader, LogSeverity_Error, "Could not open binary pixel shader for reading.");
        return;
    }

    StreamSeek(binaryVertexShader, StreamSeek_End);
    auto vsSize = StreamTell(binaryVertexShader);
    auto vs = _mm_malloc(vsSize, 16);
    StreamSeek(binaryVertexShader, 0);
    StreamRead(binaryVertexShader, vsSize, vs);
    StreamClose(binaryVertexShader);

    StreamSeek(binaryPixelShader, StreamSeek_End);
    auto psSize = StreamTell(binaryPixelShader);
    auto ps = _mm_malloc(psSize, 16);
    StreamSeek(binaryPixelShader, 0);
    StreamRead(binaryPixelShader, psSize, ps);
    StreamClose(binaryPixelShader);

    auto vsHandle = bgfx::createShader(bgfx::makeRef(vs, vsSize, free_func));
    auto psHandle = bgfx::createShader(bgfx::makeRef(ps, psSize, free_func));
    auto programHandle = bgfx::createProgram(vsHandle, psHandle, true);

    SetBgfxResourceHandle(entity, programHandle.idx);
}

LocalFunction(OnBinaryProgramValidation, void, Entity component) {
    for_entity(entity, data, BgfxBinaryProgram) {
        if(!IsDirty(entity)) continue;

        CompileProgram(entity);
    }
}

BeginUnit(BgfxShaderCache)
    BeginComponent(BgfxBinaryProgram)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxBinaryProgramRemoved, ComponentOf_BgfxBinaryProgram())
    RegisterSubscription(EventOf_Validate(), OnBinaryProgramValidation, ComponentOf_BinaryProgram())
EndUnit()
