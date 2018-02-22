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
#include <Foundation/StreamedPersistancePoint.h>
#include <File/FileStream.h>
#include <Core/String.h>
#include "PlayerContext.h"


    DefineService(PlayerContext)
    EndService()

    Entity PlayerContext, RuntimeFolder;

    Entity GetPlayerContext() {
        return PlayerContext;
    }

    int PlayerMain(int argc, char** argv, Module *modules, StringRef* assets, StringRef title) {
        InitializeModule(ModuleOf_Foundation());

        for(int i = 0; modules[i]; ++i) {
            InitializeModule(modules[i]);
        }

        SetContextTitle(PlayerContext, title);

        while(assets && *assets) {
            auto persistancePoint = CreatePersistancePoint(FormatString("/Assets/%s", *assets));
            auto fileStream = CreateFileStream(FormatString("%s/Stream", GetEntityPath(persistancePoint)));
            SetFilePath(fileStream, *assets);
            SetPersistancePointStream(persistancePoint, fileStream);

            Load(persistancePoint);
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
