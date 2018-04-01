//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Rendering/Context.h>
#include <Rendering/CommandList.h>
#include <Foundation/AppLoop.h>
#include <Foundation/FoundationModule.h>
#include <File/Folder.h>
#include <zconf.h>
#include <Core/Hierarchy.h>
#include <Foundation/PersistancePoint.h>
#include <File/FileStream.h>
#include <Core/String.h>
#include <Foundation/Stream.h>
#include <Foundation/VirtualPath.h>
#include <Rendering/RenderTarget.h>
#include "PlayerContext.h"

    Entity PlayerContext = 0, RuntimeFolder = 0;

    Entity GetPlayerContext() {
        return PlayerContext;
    }

    int PlayerMain(int argc, char** argv, Module *modules, const StringRef* virtualPathMappings, const StringRef* assets, StringRef title) {
        InitializeModule(ModuleOf_Foundation());

        while(virtualPathMappings && *virtualPathMappings) {
            char name[PATH_MAX];
            sprintf(name, "/.vpaths/%llu", (u64)*virtualPathMappings);
            auto entity = CreateEntityFromPath(name);
            SetVirtualPathTrigger(entity, *virtualPathMappings);
            virtualPathMappings++;

            char cleanPath[PATH_MAX];
            strcpy(cleanPath, *virtualPathMappings);
            CleanupPath(cleanPath);

            SetVirtualPathDestination(entity, cleanPath);
            virtualPathMappings++;

            Log(LogChannel_Core, LogSeverity_Info, "Mapping path %s to %s ...", GetVirtualPathTrigger(entity), GetVirtualPathDestination(entity));
        }

        for(int i = 0; modules[i]; ++i) {
            InitializeModule(modules[i]);
        }

        SetContextTitle(PlayerContext, title);

        while(assets && *assets) {
            char path[PATH_MAX];
			auto protocolOffset = strstr(*assets, "://");
			Assert(protocolOffset);
            snprintf(path, PATH_MAX, "/%s", protocolOffset + 3);
            auto assetStream = CreateEntityFromPath(path);
            SetStreamPath(assetStream, *assets);
            Load(assetStream);
            assets++;
        }

        while(IsEntityValid(PlayerContext) && IsPlayerContextRunning()) {
            AppUpdate();
        }

        for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
            ShutdownModule(module);
        }

        return 0;
    }

    Entity GetRuntimeFolder() {
        return RuntimeFolder;
    }

    static void OnContextClosing(Entity entity) {
        if(entity == PlayerContext) {
            DestroyEntity(PlayerContext);
        }
    }
    static void OnServiceStart(Service service) {
        PlayerContext = CreateContext(0, "PlayerContext");
        SetRenderTargetSize(PlayerContext, {800, 600});
    }

    static void OnServiceStop(Service service){
        DestroyEntity(PlayerContext);
    }

DefineService(PlayerContext)
    Subscribe(ContextClosing, OnContextClosing)
    Subscribe(PlayerContextStarted, OnServiceStart)
        Subscribe(PlayerContextStopped, OnServiceStop)
EndService()
