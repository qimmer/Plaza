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
#include "PlayerContext.h"


    DefineService(PlayerContext)
    EndService()

    Entity PlayerContext, RuntimeFolder;

    Entity GetPlayerContext() {
        return PlayerContext;
    }

    int PlayerMain(int argc, char** argv, Module *modules, const StringRef* virtualPathMappings, const StringRef* assets, StringRef title) {
        InitializeModule(ModuleOf_Foundation());

        while(virtualPathMappings && *virtualPathMappings) {
            auto entity = CreateVirtualPath(FormatString("/.vpaths/%llu", (u64)*virtualPathMappings));
            SetVirtualPathTrigger(entity, *virtualPathMappings);
            virtualPathMappings++;
            SetVirtualPathDestination(entity, CleanupPath(*virtualPathMappings));
            virtualPathMappings++;

            Log(LogChannel_Core, LogSeverity_Info, "Mapping path %s to %s ...", GetVirtualPathTrigger(entity), GetVirtualPathDestination(entity));
        }

        for(int i = 0; modules[i]; ++i) {
            InitializeModule(modules[i]);
        }

        SetContextTitle(PlayerContext, title);

        while(assets && *assets) {
            auto assetStream = CreateStream(FormatString("/%s", strstr(*assets, "://") + 3));
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
    static bool ServiceStart() {
        PlayerContext = CreateContext("/PlayerContext");
        SetContextSize(PlayerContext, {800, 600});

        SubscribeContextClosing(OnContextClosing);

        return true;
    }

    static bool ServiceStop() {
        UnsubscribeContextClosing(OnContextClosing);
        DestroyEntity(PlayerContext);

        return true;
    }
