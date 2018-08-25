//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Rendering/RenderContext.h>
#include <Rendering/CommandList.h>
#include <Foundation/AppLoop.h>
#include <Foundation/FoundationModule.h>
#include <File/Folder.h>
#include <zconf.h>
#include <Core/Node.h>
#include <Foundation/PersistancePoint.h>
#include <File/FileStream.h>
#include <Core/String.h>
#include <Foundation/Stream.h>
#include <Foundation/VirtualPath.h>
#include <Rendering/RenderTarget.h>
#include "PlayerContext.h"

    Entity RuntimeFolder = 0;

API_EXPORT int PlayerMain(int argc, char** argv, Module *modules, const StringRef* virtualPathMappings, const StringRef* assets) {
        InitializeModule(ModuleOf_Foundation());

        while(virtualPathMappings && *virtualPathMappings) {
            char name[PathMax];
            sprintf(name, "/.vpaths/%llu", (u64)*virtualPathMappings);
            auto entity = CreateEntityFromPath(name);
            SetVirtualPathTrigger(entity, *virtualPathMappings);
            virtualPathMappings++;

            char cleanPath[PathMax];
            strcpy(cleanPath, *virtualPathMappings);
            CleanupPath(cleanPath);

            SetVirtualPathDestination(entity, cleanPath);
            virtualPathMappings++;

            Log(LogChannel_Core, LogSeverity_Info, "Mapping path %s to %s ...", GetVirtualPathTrigger(entity), GetVirtualPathDestination(entity));
        }

        for(int i = 0; modules[i]; ++i) {
            InitializeModule(modules[i]);
        }

        while(assets && *assets) {
            char path[PathMax];
			auto protocolOffset = strstr(*assets, "://");
			Assert(protocolOffset);
            snprintf(path, PathMax, "/%s", protocolOffset + 3);
            auto assetStream = CreateEntityFromPath(path);
            SetStreamPath(assetStream, *assets);
            Load(assetStream);
            assets++;
        }

        while(IsPlayerContextRunning()) {
            AppUpdate();
        }

        for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
            ShutdownModule(module);
        }

        return 0;
    }

API_EXPORT Entity GetRuntimeFolder() {
        return RuntimeFolder;
    }

DefineService(PlayerContext)
EndService()
