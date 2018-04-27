//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/String.h>
#include <Foundation/Stream.h>
#include <Core/Hashing.h>
#include <Core/Dictionary.h>
#include <File/Folder.h>
#include "Program.h"
#include "Shader.h"
#include "BinaryShader.h"
#include "ShaderVariation.h"

struct Program {
    Entity ProgramBinaryVertexShader, ProgramBinaryPixelShader;
};

DefineComponent(Program)
    DefinePropertyReactive(Entity, ProgramBinaryVertexShader)
    DefinePropertyReactive(Entity, ProgramBinaryPixelShader)
EndComponent()

DefineComponentPropertyReactive(Program, Entity, ProgramBinaryVertexShader);
DefineComponentPropertyReactive(Program, Entity, ProgramBinaryPixelShader)

struct ProgramCombination {
    Entity vertexShader, pixelShader, shaderVariation;
    u8 profile;
    Entity program;
};

Lookup<u32, ProgramCombination> ProgramLookup;

Entity ProgramRoot;

static u32 HashCombination(Entity vertexShader, Entity pixelShader, u8 profile, Entity shaderVariation) {
    u64 mem[] = {
        vertexShader,
        pixelShader,
        profile,
        shaderVariation
    };

    return HashCreate(mem, sizeof(u64) * 4, 0xaDeafDae);
}

void GetBinaryShaderStreamPath(Entity shader, u8 profile, Entity shaderVariation, char *pathOut, u32 len) {
    auto originalPath = GetStreamPath(shader);
    auto fileName = GetFileName(originalPath);

    auto flagIndex = GetEnumFlagIndexByValue(EnumOf_ShaderProfile(), profile);
    char variationName[PATH_MAX];
    strcpy(variationName, "default");

    if(IsEntityValid(shaderVariation)) {
        strncpy(variationName, GetShaderVariationDefines(shaderVariation), PATH_MAX);
        auto varNameLen = strlen(variationName);
        for(auto i = 0; i < varNameLen; ++i) {
            variationName[i] = tolower(variationName[i]);
            if(!isalnum(variationName[i])) variationName[i] = '_';
        }
    }

    snprintf(pathOut, len, "res://shadercache/%s.%s_%s.bin", fileName, GetEnumFlagName(EnumOf_ShaderProfile(), flagIndex), variationName);
}

static Entity CreateCompositionProgram(Entity vertexShader, Entity pixelShader, u8 profile, Entity shaderVariation) {
    auto hash = HashCombination(vertexShader, pixelShader, profile, shaderVariation);
    char name[PATH_MAX];
    snprintf(name, PATH_MAX, "Program_%lu", hash);
    auto program = CreateProgram(ProgramRoot, name);

    auto binaryVertexShader = CreateBinaryShader(program, "VertexShaderBinary");
    GetBinaryShaderStreamPath(vertexShader, profile, shaderVariation, name, PATH_MAX);
    SetStreamPath(binaryVertexShader, name);
    SetBinaryShaderProfile(binaryVertexShader, profile);
    SetBinaryShaderVariation(binaryVertexShader, shaderVariation);
    SetSourceShader(binaryVertexShader, vertexShader);
    SetProgramBinaryVertexShader(program, binaryVertexShader);

    auto binaryPixelShader = CreateBinaryShader(program, "PixelShaderBinary");
    GetBinaryShaderStreamPath(pixelShader, profile, shaderVariation, name, PATH_MAX);
    SetStreamPath(binaryPixelShader, name);
    SetBinaryShaderProfile(binaryPixelShader, profile);
    SetBinaryShaderVariation(binaryPixelShader, shaderVariation);
    SetSourceShader(binaryPixelShader, pixelShader);
    SetProgramBinaryPixelShader(program, binaryPixelShader);

    return program;
}

Entity ResolveProgram(Entity vertexShader, Entity pixelShader, u8 profile, Entity shaderVariation) {
    auto hash = HashCombination(vertexShader, pixelShader, profile, shaderVariation);

    auto it = ProgramLookup.find(hash);
    if(it == ProgramLookup.end()) {
        auto program = CreateCompositionProgram(vertexShader, pixelShader, profile, shaderVariation);

        ProgramCombination combi = {
            vertexShader,
            pixelShader,
            shaderVariation,
            profile,
            program
        };

        ProgramLookup[hash] = combi;

        return program;
    }

    return it->second.program;
};

DefineService(Program)
    ServiceEntity(ProgramRoot, "/.Program", NULL)
EndService()